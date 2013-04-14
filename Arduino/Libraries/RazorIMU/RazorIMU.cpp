//
//  RazorIMU.cpp
//  RazorIMU
//
//  Created by Onishi Yoshito on 2/13/13.
//  Copyright (c) 2013 Onishi Yoshito. All rights reserved.
//

#include "RazorIMU.h"
#include <EEPROM.h>
#include <Wire.h>
#include "vec3.h"

// OUTPUT OPTIONS
/*****************************************************************/
// Select if serial continuous streaming output is enabled per default on startup.
#define OUTPUT__STARTUP_STREAM_ON false  // true or false

//// If set true, an error message will be output if we fail to read sensor data.
//// Message format: "!ERR: reading <sensor>", followed by "\r\n".
//boolean output_errors = false;  // true or false

// DEBUG OPTIONS
/*****************************************************************/
// Print elapsed time after each I/O loop
#define DEBUG__PRINT_LOOP_TIME false


const int RazorIMU::OUTPUT__DATA_INTERVAL   = 20;

// Gain for gyroscope (ITG-3200)
const float RazorIMU::GYRO_GAIN             = 0.06957f;// Same gain on all axes

// DCM parameters
const float RazorIMU::Kp_ROLLPITCH          = 0.02f;
const float RazorIMU::Ki_ROLLPITCH          = 0.00002f;
const float RazorIMU::Kp_YAW                = 1.2f;
const float RazorIMU::Ki_YAW                = 0.00002f;

// Stuff
// Pin number of status LED
const int RazorIMU::STATUS_LED_PIN          = 9;
// "1G reference" used for DCM filter and accelerometer calibration
const float RazorIMU::GRAVITY               = 256.0f;

/// motioner
const float RazorIMU::MPU6050_ACCEL_SCALE   = 16384.0f/256.0f;

/// EEPROM
/// accel min/max 3x2, gyro offset x 3, magn ellipsoid center/transform 3+9
const int RazorIMU::EEPROM_SIZE             = sizeof(float) * (6 + 3 + 3 + 9);
const int RazorIMU::EEPROM_SIGNATURE        = 0x24;
const int RazorIMU::EEPROM_BASE             = 0x80; // 128
const int RazorIMU::EEPROM_NODE_ID          = EEPROM_BASE+1;

// Calculate the scaled gyro readings in radians per second
#define GYRO_SCALED_RAD(x) (x * TO_RAD(GYRO_GAIN))
// *pi/180
#define TO_RAD(x) (x * 0.01745329252)
// *180/pi
#define TO_DEG(x) (x * 57.2957795131)


/// motioner
//--------------------------------------------------
RazorIMU::RazorIMU() :
has_user_calibration(false)
{
    /// motioner
    
    /// init members
    output_mode = OUTPUT__MODE_ANGLES;
    output_format = OUTPUT__FORMAT_TEXT;
            
    // DCM variables
    for (int i=0; i<3; i++) {
        Accel_Vector[i] = 0;
        Gyro_Vector[i] = 0;
        Omega_Vector[i] = 0;
        Omega_P[i] = 0;
        Omega_I[i] = 0;
        Omega[i] = 0;
        errorRollPitch[i] = 0;
        errorYaw[i] = 0;
        for (int j=0; j<3; j++) {
            DCM_Matrix[i][j] = 0;
            Update_Matrix[i][j] = 0;
            Temporary_Matrix[i][j] = 0;
        }
    }
    
    ax = ay = az = gx = gy = gz = 0;
}

RazorIMU::~RazorIMU()
{
    
}

void RazorIMU::loadCalibration()
{
    /// load
    Serial.println("Loading calibration data...");
    
    bool eeprom = false;
    
    if(EEPROM.read(EEPROM_BASE) == EEPROM_SIGNATURE) { // check if signature is ok so we have good data
        eepromHead = EEPROM_BASE+1; // reset location
        
        eeprom_read_var(sizeof(accel_x_min), (byte *) &accel_x_min);
        eeprom_read_var(sizeof(accel_x_max), (byte *) &accel_x_max);
        
        eeprom_read_var(sizeof(accel_y_min), (byte *) &accel_y_min);
        eeprom_read_var(sizeof(accel_y_max), (byte *) &accel_y_max);
        
        eeprom_read_var(sizeof(accel_z_min), (byte *) &accel_z_min);
        eeprom_read_var(sizeof(accel_z_max), (byte *) &accel_z_max);
        
        eeprom_read_var(sizeof(gyro_average_offset_x), (byte *) &gyro_average_offset_x);
        eeprom_read_var(sizeof(gyro_average_offset_y), (byte *) &gyro_average_offset_y);
        eeprom_read_var(sizeof(gyro_average_offset_z), (byte *) &gyro_average_offset_z);
        
        for (int i=0; i<3; i++)
            eeprom_read_var(sizeof(magn_ellipsoid_center[i]),
                            (byte *) &magn_ellipsoid_center[i]);
        
        for (int i=0; i<3; i++) {
            for (int j=0; j<3; j++) {
                eeprom_read_var(sizeof(magn_ellipsoid_transform[i][j]),
                                (byte *) &magn_ellipsoid_transform[i][j]);
            }
        }
        eeprom = true;
    }
    else {
        /// load deafault calibration
        accel_x_min = -250.0f;
        accel_x_max =  250.0f;
        accel_y_min = -250.0f;
        accel_y_max =  250.0f;
        accel_z_min = -250.0f;
        accel_z_max =  250.0f;
        
        gyro_average_offset_x = 0.0f;
        gyro_average_offset_y = 0.0f;
        gyro_average_offset_z = 0.0f;
        
        for (int i=0; i<3; i++) {
            magn_ellipsoid_center[i] = 0.0f;
            for (int j=0; j<3; j++) {
                magn_ellipsoid_transform[i][j] = 0.0f;
            }
        }
        
        /// identity matrix
        for (int i=0; i<3; i++) {
            magn_ellipsoid_transform[i][i] = 1.0f;
        }
    }
    
    /// calcurate
    accel_x_offset = (accel_x_min + accel_x_max) / 2.0f;
    accel_y_offset = (accel_y_min + accel_y_max) / 2.0f;
    accel_z_offset = (accel_z_min + accel_z_max) / 2.0f;
    accel_x_scale = GRAVITY / (accel_x_max - accel_x_offset);
    accel_y_scale = GRAVITY / (accel_y_max - accel_y_offset);
    accel_z_scale = GRAVITY / (accel_z_max - accel_z_offset);
    
    Serial.print("calibration loaded from ");
    Serial.println(eeprom ? "EEPROM" : "default values");
}

void RazorIMU::writeCalibration()
{
    Serial.println("saving calibration...");
    
    /// signature
    EEPROM.write(EEPROM_BASE, EEPROM_SIGNATURE);
    
    eepromHead = EEPROM_BASE+1; // reset location
    
    /// acce
    eeprom_write_var(sizeof(accel_x_min), (byte *) &accel_x_min);
    eeprom_write_var(sizeof(accel_x_max), (byte *) &accel_x_max);
    
    eeprom_write_var(sizeof(accel_y_min), (byte *) &accel_y_min);
    eeprom_write_var(sizeof(accel_y_max), (byte *) &accel_y_max);
    
    eeprom_write_var(sizeof(accel_z_min), (byte *) &accel_z_min);
    eeprom_write_var(sizeof(accel_z_max), (byte *) &accel_z_max);
    
    /// gyro
    eeprom_write_var(sizeof(gyro_average_offset_x), (byte *) &gyro_average_offset_x);
    eeprom_write_var(sizeof(gyro_average_offset_y), (byte *) &gyro_average_offset_y);
    eeprom_write_var(sizeof(gyro_average_offset_z), (byte *) &gyro_average_offset_z);
    
    /// magn
    for (int i=0; i<3; i++)
        eeprom_write_var(sizeof(magn_ellipsoid_center[i]),
                         (byte *) &magn_ellipsoid_center[i]);
    
    for (int i=0; i<3; i++) {
        for (int j=0; j<3; j++) {
            eeprom_write_var(sizeof(magn_ellipsoid_transform[i][j]),
                             (byte *) &magn_ellipsoid_transform[i][j]);
        }
    }
    
    //mFreeIMU.calLoad(); // reload calibration
    // toggle LED after calibration store.
    digitalWrite(STATUS_LED_PIN, HIGH);
    delay(1000);
    digitalWrite(STATUS_LED_PIN, LOW);
    
    Serial.println("finished");
}

void RazorIMU::printCalibration()
{
    Serial.println("-calibration-");
    Serial.print("acce range = {");
    Serial.print(accel_x_min);
    Serial.print("/");
    Serial.print(accel_x_max);
    Serial.print(", ");
    Serial.print(accel_y_min);
    Serial.print("/");
    Serial.print(accel_y_max);
    Serial.print(", ");
    Serial.print(accel_z_min);
    Serial.print("/");
    Serial.print(accel_z_max);
    Serial.println("}");
    
    Serial.print("magn center = {");
    Serial.print(magn_ellipsoid_center[0]);
    Serial.print(", ");
    Serial.print(magn_ellipsoid_center[1]);
    Serial.print(", ");
    Serial.print(magn_ellipsoid_center[2]);
    Serial.println("}");
    
    Serial.println("magn transform =");
    Serial.println("{");
    for (int i=0; i<3; i++) {
        Serial.print("    {");
        for (int j=0; j<3; j++) {
            Serial.print(magn_ellipsoid_transform[i][j]);
            Serial.print(", ");
        }
        Serial.println("},");
    }
    Serial.println("}");
    
    Serial.print("gyro offset = {");
    Serial.print(gyro_average_offset_x);
    Serial.print(", ");
    Serial.print(gyro_average_offset_y);
    Serial.print(", ");
    Serial.print(gyro_average_offset_z);
    Serial.println("}");
}

void RazorIMU::eeprom_read_var(uint8_t size, byte * var)
{
    for(uint8_t i = 0; i<size; i++) {
        var[i] = EEPROM.read(eepromHead + i);
    }
    eepromHead += size;
}

void RazorIMU::eeprom_write_var(uint8_t size, byte * var)
{
    for(uint8_t i = 0; i<size; i++) {
        EEPROM.write(eepromHead + i, var[i]);
    }
    eepromHead += size;
}

void RazorIMU::readGyroCalibrationFromSerial()
{
    //Serial.println("Reading gyroscope calibration");
    
    while (Serial.available() < sizeof(float)*(3)) ; /// wait until data rady
    
    FloatAndByte offset[3];
    
    for (int i=0; i<3; i++) {
        for (int b=0; b<sizeof(float); b++) {
            offset[i].b[b] = Serial.read();
        }
    }
    
    gyro_average_offset_x = offset[0].f;
    gyro_average_offset_y = offset[1].f;
    gyro_average_offset_z = offset[2].f;
    
    //Serial.println("Finished to read gyroscope calibration");
}

void RazorIMU::readMagnCalibrationFromSerial()
{
    //Serial.println("Reading magnetometer calibration");
    
    while (Serial.available() < sizeof(float)*(3+9)) ; /// wait until data rady
    
    FloatAndByte center[3];
    
    for (int i=0; i<3; i++) {
        for (int b=0; b<sizeof(float); b++) {
            center[i].b[b] = Serial.read();
        }
        magn_ellipsoid_center[i] = center[i].f;
    }
    
    FloatAndByte trans[3][3];
    
    for (int i=0; i<3; i++) {
        for (int j=0; j<3; j++) {
            for (int b=0; b<sizeof(float); b++) {
                trans[i][j].b[b] = Serial.read();
            }
            magn_ellipsoid_transform[i][j] = trans[i][j].f;
        }
    }
    
    //Serial.println("Finished to read magnetometer calibration");
}

void RazorIMU::readAcceCalibrationFromSerial()
{
    //Serial.println("Reading accelerometer calibration");
    
    while (Serial.available() < sizeof(float)*(6)) ; /// wait until data rady
    
    FloatAndByte range[6];
    
    for (int i=0; i<6; i++) {
        for (int b=0; b<sizeof(float); b++) {
            range[i].b[b] = Serial.read();
        }
    }
    
    accel_x_min = range[0].f;
    accel_x_max = range[1].f;
    accel_y_min = range[2].f;
    accel_y_max = range[3].f;
    accel_z_min = range[4].f;
    accel_z_max = range[5].f;
    
    //Serial.println("Finished to read accelerometer calibration");
}

void RazorIMU::setup()
{
    // Init status LED
    pinMode (STATUS_LED_PIN, OUTPUT);
    digitalWrite(STATUS_LED_PIN, LOW);
    
    accelgyro = MPU60X0(MPU60X0_DEFAULT_ADDRESS);
    
    // Init sensors
    delay(50);  // Give sensors enough time to start
    
    Serial.println("init I2C");
    I2C_Init();
    Serial.println("success");
    
    // initialize device
    // initialize MPU6050
    Serial.println("init MPU6050");
    accelgyro.initialize();
    accelgyro.setI2CMasterModeEnabled(false);
    accelgyro.setI2CBypassEnabled(true);
    accelgyro.setFullScaleGyroRange(MPU60X0_GYRO_FS_2000);
    //// verify connection
    Serial.println(accelgyro.testConnection() ? "success" : "failed");
    
    Serial.println("init HMC5883L");
    Magn_Init();
    Serial.println("success");
    
    // Read sensors, init DCM algorithm
    delay(20);  // Give sensors enough time to collect data
    reset_sensor_fusion();
    
    // Init output
#if (OUTPUT__STARTUP_STREAM_ON == false)
    turn_output_stream_off();
#else
    turn_output_stream_on();
#endif
    
    loadCalibration();
}

void RazorIMU::updateSerial(int command)
{
    if (command == 'f') // request one output _f_rame
        output_single_on = true;
    else if (command == 's') // _s_ynch request
    {
        // Read ID
        byte id[2];
        id[0] = readChar();
        id[1] = readChar();
        
        // Reply with synch message
        Serial.print("#SYNCH");
        Serial.write(id, 2);
        Serial.println();
    }
    else if (command == 'o') // Set _o_utput mode
    {
        char output_param = readChar();
        if (output_param == 't') // Output angles as _t_ext
        {
            output_mode = OUTPUT__MODE_ANGLES;
            output_format = OUTPUT__FORMAT_TEXT;
        }
        else if (output_param == 'b') // Output angles in _b_inary format
        {
            output_mode = OUTPUT__MODE_ANGLES;
            output_format = OUTPUT__FORMAT_BINARY;
        }
        else if (output_param == 's') // Output _s_ensor values
        {
            char values_param = readChar();
            char format_param = readChar();
            if (values_param == 'r')  // Output _r_aw sensor values
                output_mode = OUTPUT__MODE_SENSORS_RAW;
            else if (values_param == 'c')  // Output _c_alibrated sensor values
                output_mode = OUTPUT__MODE_SENSORS_CALIB;
            else if (values_param == 'b')  // Output _b_oth sensor values (raw and calibrated)
                output_mode = OUTPUT__MODE_SENSORS_BOTH;
            
            if (format_param == 't') // Output values as _t_text
                output_format = OUTPUT__FORMAT_TEXT;
            else if (format_param == 'b') // Output values in _b_inary format
                output_format = OUTPUT__FORMAT_BINARY;
        }
        else if (output_param == '0') // Disable continuous streaming output
        {
            turn_output_stream_off();
        }
        else if (output_param == '1') // Enable continuous streaming output
        {
            turn_output_stream_on();
        }
    }
    /// motioner
    else if (command=='c') /// _c_alibration
    {
        char output_param = readChar();
        if (output_param == 'l')  // _l_oad calibration
        {
            loadCalibration();
        }
        else if (output_param == 'w') // _w_rite calibration
        {
            writeCalibration();
        }
        else if (output_param == 'o') // _o_utput calibration to serial
        {
            printCalibration();
        }
        else if (output_param == 'a') // read _a_ccelerometer calibration from serial
        {
            readAcceCalibrationFromSerial();
        }
        else if (output_param == 'm') // read _m_agnetmeter calibration from serial
        {
            readMagnCalibrationFromSerial();
        }
        else if (output_param == 'g') // read _g_yroscope calibration from serial
        {
            readGyroCalibrationFromSerial();
        }
    }
}

void RazorIMU::updateAHRS()
{
    // Time to read the sensors again?
    if((millis() - timestamp) >= OUTPUT__DATA_INTERVAL)
    {
        timestamp_old = timestamp;
        timestamp = millis();
        if (timestamp > timestamp_old)
            G_Dt = (float) (timestamp - timestamp_old) / 1000.0f; // Real time of loop run. We use this on the DCM algorithm (gyro integration time)
        else G_Dt = 0;
        
        // Update sensor readings
        read_sensors();
        
        if (output_mode == OUTPUT__MODE_ANGLES)  // Output angles
        {
            // Apply sensor calibration
            compensate_sensor_errors();
            
            // Run DCM algorithm
            Compass_Heading(); // Calculate magnetic heading
            Matrix_update();
            Normalize();
            Drift_correction();
            Euler_angles();
            
            /// motioner
            calcQuaternion();
            
            if (output_stream_on || output_single_on) output_angles();
        }
        else  // Output sensor values
        {
            if (output_stream_on || output_single_on) output_sensors();
        }
        
        output_single_on = false;
        
#if DEBUG__PRINT_LOOP_TIME == true
        Serial.print("loop time (ms) = ");
        Serial.println(millis() - timestamp);
#endif
    }
#if DEBUG__PRINT_LOOP_TIME == true
    else
    {
        Serial.println("waiting");
    }
#endif
}

void  RazorIMU::getRawAcce(float *val) const
{
    val[0] = accel[0];
    val[1] = accel[1];
    val[2] = accel[2];
}


// Compass_Heading

/**************************************************/
void RazorIMU::Compass_Heading()
{
    float mag_x;
    float mag_y;
    float cos_roll;
    float sin_roll;
    float cos_pitch;
    float sin_pitch;
    
    cos_roll = cos(roll);
    sin_roll = sin(roll);
    cos_pitch = cos(pitch);
    sin_pitch = sin(pitch);
    
    // Tilt compensated magnetic field X
    mag_x = magnetom[0] * cos_pitch + magnetom[1] * sin_roll * sin_pitch + magnetom[2] * cos_roll * sin_pitch;
    // Tilt compensated magnetic field Y
    mag_y = magnetom[1] * cos_roll - magnetom[2] * sin_roll;
    // Magnetic Heading
    MAG_Heading = atan2(-mag_y, mag_x);
}



// DCM algorithm

/**************************************************/
void RazorIMU::Normalize(void)
{
    float error=0;
    float temporary[3][3];
    float renorm=0;
    
    error= -Vector_Dot_Product(&DCM_Matrix[0][0],&DCM_Matrix[1][0])*.5; //eq.19
    
    Vector_Scale(&temporary[0][0], &DCM_Matrix[1][0], error); //eq.19
    Vector_Scale(&temporary[1][0], &DCM_Matrix[0][0], error); //eq.19
    
    Vector_Add(&temporary[0][0], &temporary[0][0], &DCM_Matrix[0][0]);//eq.19
    Vector_Add(&temporary[1][0], &temporary[1][0], &DCM_Matrix[1][0]);//eq.19
    
    Vector_Cross_Product(&temporary[2][0],&temporary[0][0],&temporary[1][0]); // c= a x b //eq.20
    
    renorm= .5 *(3 - Vector_Dot_Product(&temporary[0][0],&temporary[0][0])); //eq.21
    Vector_Scale(&DCM_Matrix[0][0], &temporary[0][0], renorm);
    
    renorm= .5 *(3 - Vector_Dot_Product(&temporary[1][0],&temporary[1][0])); //eq.21
    Vector_Scale(&DCM_Matrix[1][0], &temporary[1][0], renorm);
    
    renorm= .5 *(3 - Vector_Dot_Product(&temporary[2][0],&temporary[2][0])); //eq.21
    Vector_Scale(&DCM_Matrix[2][0], &temporary[2][0], renorm);
}

/**************************************************/
void RazorIMU::Drift_correction(void)
{
    float mag_heading_x;
    float mag_heading_y;
    float errorCourse;
    //Compensation the Roll, Pitch and Yaw drift.
    static float Scaled_Omega_P[3];
    static float Scaled_Omega_I[3];
    float Accel_magnitude;
    float Accel_weight;
    
    
    //*****Roll and Pitch***************
    
    // Calculate the magnitude of the accelerometer vector
    Accel_magnitude = sqrt(Accel_Vector[0]*Accel_Vector[0] + Accel_Vector[1]*Accel_Vector[1] + Accel_Vector[2]*Accel_Vector[2]);
    Accel_magnitude = Accel_magnitude / GRAVITY; // Scale to gravity.
    // Dynamic weighting of accelerometer info (reliability filter)
    // Weight for accelerometer info (<0.5G = 0.0, 1G = 1.0 , >1.5G = 0.0)
    Accel_weight = constrain(1 - 2*abs(1 - Accel_magnitude),0,1);  //
    
    Vector_Cross_Product(&errorRollPitch[0],&Accel_Vector[0],&DCM_Matrix[2][0]); //adjust the ground of reference
    Vector_Scale(&Omega_P[0],&errorRollPitch[0],Kp_ROLLPITCH*Accel_weight);
    
    Vector_Scale(&Scaled_Omega_I[0],&errorRollPitch[0],Ki_ROLLPITCH*Accel_weight);
    Vector_Add(Omega_I,Omega_I,Scaled_Omega_I);
    
    //*****YAW***************
    // We make the gyro YAW drift correction based on compass magnetic heading
    
    mag_heading_x = cos(MAG_Heading);
    mag_heading_y = sin(MAG_Heading);
    errorCourse=(DCM_Matrix[0][0]*mag_heading_y) - (DCM_Matrix[1][0]*mag_heading_x);  //Calculating YAW error
    Vector_Scale(errorYaw,&DCM_Matrix[2][0],errorCourse); //Applys the yaw correction to the XYZ rotation of the aircraft, depeding the position.
    
    Vector_Scale(&Scaled_Omega_P[0],&errorYaw[0],Kp_YAW);//.01proportional of YAW.
    Vector_Add(Omega_P,Omega_P,Scaled_Omega_P);//Adding  Proportional.
    
    Vector_Scale(&Scaled_Omega_I[0],&errorYaw[0],Ki_YAW);//.00001Integrator
    Vector_Add(Omega_I,Omega_I,Scaled_Omega_I);//adding integrator to the Omega_I
}

void RazorIMU::Matrix_update(void)
{
    Gyro_Vector[0]=GYRO_SCALED_RAD(gyro[0]); //gyro x roll
    Gyro_Vector[1]=GYRO_SCALED_RAD(gyro[1]); //gyro y pitch
    Gyro_Vector[2]=GYRO_SCALED_RAD(gyro[2]); //gyro z yaw
    
    Accel_Vector[0]=accel[0];
    Accel_Vector[1]=accel[1];
    Accel_Vector[2]=accel[2];
    
    Vector_Add(&Omega[0], &Gyro_Vector[0], &Omega_I[0]);  //adding proportional term
    Vector_Add(&Omega_Vector[0], &Omega[0], &Omega_P[0]); //adding Integrator term
    
#if DEBUG__NO_DRIFT_CORRECTION == true // Do not use drift correction
    Update_Matrix[0][0]=0;
    Update_Matrix[0][1]=-G_Dt*Gyro_Vector[2];//-z
    Update_Matrix[0][2]=G_Dt*Gyro_Vector[1];//y
    Update_Matrix[1][0]=G_Dt*Gyro_Vector[2];//z
    Update_Matrix[1][1]=0;
    Update_Matrix[1][2]=-G_Dt*Gyro_Vector[0];
    Update_Matrix[2][0]=-G_Dt*Gyro_Vector[1];
    Update_Matrix[2][1]=G_Dt*Gyro_Vector[0];
    Update_Matrix[2][2]=0;
#else // Use drift correction
    Update_Matrix[0][0]=0;
    Update_Matrix[0][1]=-G_Dt*Omega_Vector[2];//-z
    Update_Matrix[0][2]=G_Dt*Omega_Vector[1];//y
    Update_Matrix[1][0]=G_Dt*Omega_Vector[2];//z
    Update_Matrix[1][1]=0;
    Update_Matrix[1][2]=-G_Dt*Omega_Vector[0];//-x
    Update_Matrix[2][0]=-G_Dt*Omega_Vector[1];//-y
    Update_Matrix[2][1]=G_Dt*Omega_Vector[0];//x
    Update_Matrix[2][2]=0;
#endif
    
    Matrix_Multiply(DCM_Matrix,Update_Matrix,Temporary_Matrix); //a*b=c
    
    for(int x=0; x<3; x++) //Matrix Addition (update)
    {
        for(int y=0; y<3; y++)
        {
            DCM_Matrix[x][y]+=Temporary_Matrix[x][y];
        }
    }
}

void RazorIMU::Euler_angles(void)
{
    pitch = -asin(DCM_Matrix[2][0]);
    roll = atan2(DCM_Matrix[2][1],DCM_Matrix[2][2]);
    yaw = atan2(DCM_Matrix[1][0],DCM_Matrix[0][0]);
}

void RazorIMU::calcQuaternion(void)
{
    // Source: Gamasutra, Rotating Objects Using Quaternions
    //
    //http://www.gamasutra.com/features/programming/19980703/quaternions_01.htm
    
    float  tr, s;
    float tq[4];
    int    i, j, k;
    
    int nxt[3] = {1, 2, 0};
    
    tr = DCM_Matrix[0][0] + DCM_Matrix[1][1] + DCM_Matrix[2][2]+1.0;
    
    // check the diagonal
    if (tr > 0.0)
    {
        s = (float)sqrt (tr);
        quat.w = s / 2.0;
        s = 0.5 / s;
        quat.x = (DCM_Matrix[1][2] - DCM_Matrix[2][1]) * s;
        quat.y = (DCM_Matrix[2][0] - DCM_Matrix[0][2]) * s;
        quat.z = (DCM_Matrix[0][1] - DCM_Matrix[1][0]) * s;
    }
    else
    {
        // diagonal is negative
        i = 0;
        if (DCM_Matrix[1][1] > DCM_Matrix[0][0])
            i = 1;
        if (DCM_Matrix[2][2] > DCM_Matrix[i][i])
            i = 2;
        j = nxt[i];
        k = nxt[j];
        
        s = sqrt((DCM_Matrix[i][i] - (DCM_Matrix[j][j] + DCM_Matrix[k][k])) + 1.0);
        
        tq[i] = s * 0.5;
        
        if (s != 0.0)
            s = 0.5 / s;
        
        tq[3] = (DCM_Matrix[j][k] - DCM_Matrix[k][j]) * s;
        tq[j] = (DCM_Matrix[i][j] + DCM_Matrix[j][i]) * s;
        tq[k] = (DCM_Matrix[i][k] + DCM_Matrix[k][i]) * s;
        
        quat.x = tq[0];
        quat.y = tq[1];
        quat.z = tq[2];
        quat.w = tq[3];
    }
}

// 3D Math

/**************************************************/
// Computes the dot product of two vectors
float RazorIMU::Vector_Dot_Product(const float v1[3], const float v2[3])
{
    float result = 0;
    
    for(int c = 0; c < 3; c++)
    {
        result += v1[c] * v2[c];
    }
    
    return result;
}

// Computes the cross product of two vectors
// out has to different from v1 and v2 (no in-place)!
void RazorIMU::Vector_Cross_Product(float out[3], const float v1[3], const float v2[3])
{
    out[0] = (v1[1] * v2[2]) - (v1[2] * v2[1]);
    out[1] = (v1[2] * v2[0]) - (v1[0] * v2[2]);
    out[2] = (v1[0] * v2[1]) - (v1[1] * v2[0]);
}

// Multiply the vector by a scalar
void RazorIMU::Vector_Scale(float out[3], const float v[3], float scale)
{
    for(int c = 0; c < 3; c++)
    {
        out[c] = v[c] * scale;
    }
}

// Adds two vectors
void RazorIMU::Vector_Add(float out[3], const float v1[3], const float v2[3])
{
    for(int c = 0; c < 3; c++)
    {
        out[c] = v1[c] + v2[c];
    }
}

// Multiply two 3x3 matrices: out = a * b
// out has to different from a and b (no in-place)!
void RazorIMU::Matrix_Multiply(const float a[3][3], const float b[3][3], float out[3][3])
{
    for(int x = 0; x < 3; x++)  // rows
    {
        for(int y = 0; y < 3; y++)  // columns
        {
            out[x][y] = a[x][0] * b[0][y] + a[x][1] * b[1][y] + a[x][2] * b[2][y];
        }
    }
}

// Multiply 3x3 matrix with vector: out = a * b
// out has to different from b (no in-place)!
void RazorIMU::Matrix_Vector_Multiply(const float a[3][3], const float b[3], float out[3])
{
    for(int x = 0; x < 3; x++)
    {
        out[x] = a[x][0] * b[0] + a[x][1] * b[1] + a[x][2] * b[2];
    }
}

// Init rotation matrix using euler angles
void RazorIMU::init_rotation_matrix(float m[3][3], float yaw, float pitch, float roll)
{
    float c1 = cos(roll);
    float s1 = sin(roll);
    float c2 = cos(pitch);
    float s2 = sin(pitch);
    float c3 = cos(yaw);
    float s3 = sin(yaw);
    
    // Euler angles, right-handed, intrinsic, XYZ convention
    // (which means: rotate around body axes Z, Y', X'')
    m[0][0] = c2 * c3;
    m[0][1] = c3 * s1 * s2 - c1 * s3;
    m[0][2] = s1 * s3 + c1 * c3 * s2;
    
    m[1][0] = c2 * s3;
    m[1][1] = c1 * c3 + s1 * s2 * s3;
    m[1][2] = c1 * s2 * s3 - c3 * s1;
    
    m[2][0] = -s2;
    m[2][1] = c2 * s1;
    m[2][2] = c1 * c2;
}

// Output

/**************************************************/
// Output angles: yaw, pitch, roll
void RazorIMU::output_angles()
{
    if (output_format == OUTPUT__FORMAT_BINARY)
    {
        float ypr[3];
        ypr[0] = TO_DEG(yaw);
        ypr[1] = TO_DEG(pitch);
        ypr[2] = TO_DEG(roll);
        Serial.write((byte*) ypr, 12);  // No new-line
    }
    else if (output_format == OUTPUT__FORMAT_TEXT)
    {
        Serial.print("#YPR=");
        Serial.print(TO_DEG(yaw)); Serial.print(",");
        Serial.print(TO_DEG(pitch)); Serial.print(",");
        Serial.print(TO_DEG(roll)); Serial.println();
    }
}

void RazorIMU::output_sensors_text(char raw_or_calibrated)
{
    Serial.print("#A-"); Serial.print(raw_or_calibrated); Serial.print('=');
    Serial.print(accel[0]); Serial.print(",");
    Serial.print(accel[1]); Serial.print(",");
    Serial.print(accel[2]); Serial.println();
    
    Serial.print("#M-"); Serial.print(raw_or_calibrated); Serial.print('=');
    Serial.print(magnetom[0]); Serial.print(",");
    Serial.print(magnetom[1]); Serial.print(",");
    Serial.print(magnetom[2]); Serial.println();
    
    Serial.print("#G-"); Serial.print(raw_or_calibrated); Serial.print('=');
    Serial.print(gyro[0]); Serial.print(",");
    Serial.print(gyro[1]); Serial.print(",");
    Serial.print(gyro[2]); Serial.println();
}

void RazorIMU::output_sensors_binary()
{
    Serial.write((byte*) accel, 12);
    Serial.write((byte*) magnetom, 12);
    Serial.write((byte*) gyro, 12);
}

void RazorIMU::output_sensors()
{
    if (output_mode == OUTPUT__MODE_SENSORS_RAW)
    {
        if (output_format == OUTPUT__FORMAT_BINARY)
            output_sensors_binary();
        else if (output_format == OUTPUT__FORMAT_TEXT)
            output_sensors_text('R');
    }
    else if (output_mode == OUTPUT__MODE_SENSORS_CALIB)
    {
        // Apply sensor calibration
        compensate_sensor_errors();
        
        if (output_format == OUTPUT__FORMAT_BINARY)
            output_sensors_binary();
        else if (output_format == OUTPUT__FORMAT_TEXT)
            output_sensors_text('C');
    }
    else if (output_mode == OUTPUT__MODE_SENSORS_BOTH)
    {
        if (output_format == OUTPUT__FORMAT_BINARY)
        {
            output_sensors_binary();
            compensate_sensor_errors();
            output_sensors_binary();
        }
        else if (output_format == OUTPUT__FORMAT_TEXT)
        {
            output_sensors_text('R');
            compensate_sensor_errors();
            output_sensors_text('C');
        }
    }
}

// Sensor

/**************************************************/
// I2C code to read the sensors

// Sensor I2C addresses
#define ACCEL_ADDRESS ((int) 0x53) // 0x53 = 0xA6 / 2
#define MAGN_ADDRESS  ((int) 0x1E) // 0x1E = 0x3C / 2
#define GYRO_ADDRESS  ((int) 0x68) // 0x68 = 0xD0 / 2

// Arduino backward compatibility macros
#if ARDUINO >= 100
#define WIRE_SEND(b) Wire.write((byte) b)
#define WIRE_RECEIVE() Wire.read()
#else
#define WIRE_SEND(b) Wire.send(b)
#define WIRE_RECEIVE() Wire.receive()
#endif


void RazorIMU::I2C_Init()
{
    Wire.begin();
}

void RazorIMU::Magn_Init()
{
    Wire.beginTransmission(MAGN_ADDRESS);
    WIRE_SEND(0x02);
    WIRE_SEND(0x00);  // Set continuous mode (default 10Hz)
    Wire.endTransmission();
    delay(5);
    
    Wire.beginTransmission(MAGN_ADDRESS);
    WIRE_SEND(0x00);
    WIRE_SEND(0b00011000);  // Set 50Hz
    Wire.endTransmission();
    delay(5);
}

void RazorIMU::Read_Magn()
{
    int i = 0;
    byte buff[6];
    
    Wire.beginTransmission(MAGN_ADDRESS);
    WIRE_SEND(0x03);  // Send address to read from
    Wire.endTransmission();
    
    Wire.beginTransmission(MAGN_ADDRESS);
    Wire.requestFrom(MAGN_ADDRESS, 6);  // Request 6 bytes
    while(Wire.available())  // ((Wire.available())&&(i<6))
    {
        buff[i] = WIRE_RECEIVE();  // Read one byte
        i++;
    }
    Wire.endTransmission();
    
    if (i == 6)  // All bytes received?
    {
        //using HMC5883L magnetometer
        // MSB byte first, then LSB; Y and Z reversed: X, Z, Y
        magnetom[0] = -1 * ((((int) buff[0]) << 8) | buff[1]);    // X axis (internal sensor x axis)
        magnetom[1] =  1 * ((((int) buff[4]) << 8) | buff[5]);    // Y axis (internal sensor -y axis)
        magnetom[2] =  1 * ((((int) buff[2]) << 8) | buff[3]);    // Z axis (internal sensor -z axis)
    }
}


// Utils

/**************************************************/
void RazorIMU::read_sensors() {
    //Read_Gyro(); // Read gyroscope
    //Read_Accel(); // Read accelerometer
    accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    accel[0] = -ax/MPU6050_ACCEL_SCALE;
    accel[1] = ay/MPU6050_ACCEL_SCALE;
    accel[2] = az/MPU6050_ACCEL_SCALE;
    //accel[0] = 0.0f;
    //accel[1] = 0.0f;
    //accel[2] = 0.0f;
    gyro[0] = gx;
    gyro[1] = -gy;
    gyro[2] = -gz;
    //gyro[0] = 0.0f;
    //gyro[1] = 0.0f;
    //gyro[2] = 0.0f;
    
    Read_Magn(); // Read magnetometer
    //magnetom[0] = 0.0f;
    //magnetom[1] = 0.0f;
    //magnetom[2] = 0.0f;
}

// Read every sensor and record a time stamp
// Init DCM with unfiltered orientation
// TODO re-init global vars?
void RazorIMU::reset_sensor_fusion() {
    float temp1[3];
    float temp2[3];
    float xAxis[] = {1.0f, 0.0f, 0.0f};
    
    read_sensors();
    timestamp = millis();
    
    // GET PITCH
    // Using y-z-plane-component/x-component of gravity vector
    pitch = -atan2(accel[0], sqrt(accel[1] * accel[1] + accel[2] * accel[2]));
	
    // GET ROLL
    // Compensate pitch of gravity vector
    Vector_Cross_Product(temp1, accel, xAxis);
    Vector_Cross_Product(temp2, xAxis, temp1);
    // Normally using x-z-plane-component/y-component of compensated gravity vector
    // roll = atan2(temp2[1], sqrt(temp2[0] * temp2[0] + temp2[2] * temp2[2]));
    // Since we compensated for pitch, x-z-plane-component equals z-component:
    roll = atan2(temp2[1], temp2[2]);
    
    // GET YAW
    Compass_Heading();
    yaw = MAG_Heading;
    
    // Init rotation matrix
    init_rotation_matrix(DCM_Matrix, yaw, pitch, roll);
}

// Apply calibration to raw sensor readings
void RazorIMU::compensate_sensor_errors() {
    // Compensate accelerometer error
    accel[0] = (accel[0] - accel_x_offset) * accel_x_scale;
    accel[1] = (accel[1] - accel_y_offset) * accel_y_scale;
    accel[2] = (accel[2] - accel_z_offset) * accel_z_scale;
    
    // Compensate magnetometer error
    // CALIBRATION__MAGN_USE_EXTENDED == true
    for (int i = 0; i < 3; i++)
        magnetom_tmp[i] = magnetom[i] - magn_ellipsoid_center[i];
    Matrix_Vector_Multiply(magn_ellipsoid_transform, magnetom_tmp, magnetom);
    
    // Compensate gyroscope error
    gyro[0] -= gyro_average_offset_x;
    gyro[1] -= gyro_average_offset_y;
    gyro[2] -= gyro_average_offset_z;
}

void RazorIMU::turn_output_stream_on()
{
    output_stream_on = true;
    digitalWrite(STATUS_LED_PIN, HIGH);
}

void RazorIMU::turn_output_stream_off()
{
    output_stream_on = false;
    digitalWrite(STATUS_LED_PIN, LOW);
}

// Blocks until another byte is available on serial port
char RazorIMU::readChar()
{
    while (Serial.available() < 1) { } // Block
    return Serial.read();
}


