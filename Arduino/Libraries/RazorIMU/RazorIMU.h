//
//  RazorIMU.h
//  RazorIMU
//
//  Created by Onishi Yoshito on 2/13/13.
//  Copyright (c) 2013 Onishi Yoshito. All rights reserved.
//

#ifndef __RazorIMU__RazorIMU__
#define __RazorIMU__RazorIMU__

#include "Arduino.h"

#include "I2Cdev.h"
#include "MPU60X0.h"
#include "Quaternion.h"


class RazorIMU {
public:
    RazorIMU();
    ~RazorIMU();
    
    /// motioner
    //--------------------------------------------------
    void setup();

    void updateSerial(int command);
    void updateAHRS();
    
    void  getRawAcce(float *val) const;
    
    void calcQuaternion(void);
    
    const Quaternion &getQuaternion() const { return quat; };
    
    //inline float getYaw() const { return yaw; }
    //inline float getPitch() const { return pitch; }
    //inline float getRoll() const { return roll; }
    
private:
    enum OutputMode {
        // Output mode definitions (do not change)
        // Outputs yaw/pitch/roll in degrees
        OUTPUT__MODE_ANGLES 			= 1,
        // Outputs calibrated sensor values for all 9 axes
        OUTPUT__MODE_SENSORS_CALIB 		= 2,
        // Outputs raw (uncalibrated) sensor values for all 9 axes
        OUTPUT__MODE_SENSORS_RAW 		= 3,
        // Outputs calibrated AND raw sensor values for all 9 axes
        OUTPUT__MODE_SENSORS_BOTH 		= 4,
    };
    
    enum OutputFormat {
        // Output format definitions (do not change)
        // Outputs data as text
        OUTPUT__FORMAT_TEXT		= 0,
        // Outputs data as binary float
        OUTPUT__FORMAT_BINARY	= 1,
    };
    
private:
    static const float GYRO_GAIN;
    static const float Kp_ROLLPITCH;
    static const float Ki_ROLLPITCH;
    static const float Kp_YAW;
    static const float Ki_YAW;
    
    static const int STATUS_LED_PIN;
    static const float GRAVITY;
    static const float MPU6050_ACCEL_SCALE;
    
    // Sensor data output interval in milliseconds
    // This may not work, if faster than 20ms (=50Hz)
    // Code is tuned for 20ms, so better leave it like that
    static const int OUTPUT__DATA_INTERVAL; // in milliseconds
    
    /// motioner
    //--------------------------------------------------
    
    // SENSOR CALIBRATION
    /*****************************************************************/
    // How to calibrate? Read the tutorial at ___________
    
    /// eeprom
    static const int EEPROM_SIZE;
    static const int EEPROM_SIGNATURE;
    static const int EEPROM_BASE;
    static const int EEPROM_NODE_ID;
    
private:
    void Compass_Heading();
    void Normalize(void);
    void Drift_correction(void);
    void Matrix_update(void);
    void Euler_angles(void);
    
    float Vector_Dot_Product(const float v1[3], const float v2[3]);
    void Vector_Cross_Product(float out[3], const float v1[3], const float v2[3]);
    void Vector_Scale(float out[3], const float v[3], float scale);
    void Vector_Add(float out[3], const float v1[3], const float v2[3]);
    void Matrix_Multiply(const float a[3][3], const float b[3][3], float out[3][3]);
    void Matrix_Vector_Multiply(const float a[3][3], const float b[3], float out[3]);
    void init_rotation_matrix(float m[3][3], float yaw, float pitch, float roll);
    
    void output_angles();
    void output_sensors_text(char raw_or_calibrated);
    void output_sensors_binary();
    void output_sensors();
    
    void I2C_Init();
    void Magn_Init();
    void Read_Magn();
    
    void read_sensors();
    void reset_sensor_fusion();
    void compensate_sensor_errors();
    void check_reset_calibration_session();
    void turn_output_stream_on();
    void turn_output_stream_off();
    char readChar();
    
    /// motioner
    //--------------------------------------------------
    void loadCalibration();
    void writeCalibration();
    void printCalibration();
    void eeprom_read_var(uint8_t size, byte * var);
    void eeprom_write_var(uint8_t size, byte * var);
    
    void readAcceCalibrationFromSerial();
    void readMagnCalibrationFromSerial();
    void readGyroCalibrationFromSerial();
    
private:
    union FloatAndByte {
        float f;
        byte b[4];
    };
    
private:
    
    // Select your startup output mode and format here!
    int output_mode;
    int output_format;
        
    // Sensor variables
    float accel[3];  // Actually stores the NEGATED acceleration (equals gravity, if board not moving).
    
    float magnetom[3];
    float magnetom_tmp[3];
    
    float gyro[3];
    
    // DCM variables
    float MAG_Heading;
    float Accel_Vector[3]; // Store the acceleration in a vector
    float Gyro_Vector[3]; // Store the gyros turn rate in a vector
    float Omega_Vector[3]; // Corrected Gyro_Vector data
    float Omega_P[3]; // Omega Proportional correction
    float Omega_I[3]; // Omega Integrator
    float Omega[3];
    float errorRollPitch[3];
    float errorYaw[3];
    float DCM_Matrix[3][3];
    float Update_Matrix[3][3];
    float Temporary_Matrix[3][3];
    
    // Euler angles
    float yaw;
    float pitch;
    float roll;
    
    // DCM timing in the main loop
    unsigned long timestamp;
    unsigned long timestamp_old;
    float G_Dt; // Integration time for DCM algorithm
    
    // More output-state variables
    boolean output_stream_on;
    boolean output_single_on;
    
    /// motioner
    //--------------------------------------------------
    MPU60X0 accelgyro;
    int ax, ay, az, gx, gy, gz;
    
    /// calibration
    bool has_user_calibration;
    
    float magn_ellipsoid_center[3];
    float magn_ellipsoid_transform[3][3];
    
    float accel_x_min;
    float accel_x_max;
    float accel_y_min;
    float accel_y_max;
    float accel_z_min;
    float accel_z_max;
    
    float gyro_average_offset_x;
    float gyro_average_offset_y;
    float gyro_average_offset_z;
    
    // Sensor calibration scale and offset values
    float accel_x_offset;
    float accel_y_offset;
    float accel_z_offset;
    float accel_x_scale;
    float accel_y_scale;
    float accel_z_scale;
    
    int eepromHead;
    
    Quaternion quat;
};

#endif /* defined(__RazorIMU__RazorIMU__) */
