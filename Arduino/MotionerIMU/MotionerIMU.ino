///
/// MotionerIMU firmware
///
/// Developed by Onishi Yoshito
///
/// IMU node firmware for MOTIONER which is an opensource inertial motion capture system
/// https://github.com/YCAMInterlab/MOTIONER
/// Using a part of FreeIMU hardware, canduino library, Razor AHRS Firmware v1.4.1 and I2Cdevlib
/// See also the follwing sites
/// http://www.varesano.net/topic/freeimu
/// http://code.google.com/p/canduino/
/// https://dev.qu.tu-berlin.de/projects/sf-razor-9dof-ahrs
/// http://www.i2cdevlib.com/
///
/// Copyright Onishi Yoshito and YCAM InterLab all rights reserved
/// This Arduino sketch is published under the GNU GENERAL PUBLIC LICENSE Version 3
/// 
/// version: 0.2.2
///
/// -- MotionerIMU serial command --
/// "@q" - output current quaternion
/// "@n" - change mode to writting joint id to Arduino EEPROM
/// joint id are from 0 to 18
/// and a command for set joint id is to type single character after typed @n from 'a' to 'r'
/// 'a' mean 0, 'b' mean 1, 'c' mean 2 ...... 'r' mean 17
/// so we need to type like @na (set joint id to JOINT_HIPS)
/// 
/// MOTIONER joint id and commands
///
/// - joint name -        - index -  - command -
/// JOINT_HIPS              = 0,       @na
/// JOINT_ABDOMEN           = 1,       @nb
/// JOINT_CHEST             = 2,       @nc
/// JOINT_NECK              = 3,       @nd
///   
/// JOINT_LEFT_HIP          = 4,       @ne
/// JOINT_LEFT_KNEE         = 5,       @nf
/// JOINT_LEFT_ANKLE        = 6,       @ng
///   
/// JOINT_RIGHT_HIP         = 7,       @nh
/// JOINT_RIGHT_KNEE        = 8,       @ni
/// JOINT_RIGHT_ANKLE       = 9,       @nj
///   
/// JOINT_LEFT_COLLAR       = 10,      @nk
/// JOINT_LEFT_SHOULDER     = 11,      @nl
/// JOINT_LEFT_ELBOW        = 12,      @nm
/// JOINT_LEFT_WRIST        = 13,      @nn
///   
/// JOINT_RIGHT_COLLAR      = 14,      @no
/// JOINT_RIGHT_SHOULDER    = 15,      @np
/// JOINT_RIGHT_ELBOW       = 16,      @nq
/// JOINT_RIGHT_WRIST       = 17,      @nr
///   
/// NUM_JOINTS              = 18,
///
/// -- Extended RazorAHRS serial command --
/// "#co" - output calibration data on Arduino EEPROM
///
///


/// MotionerIMU using the follwing libraries
#include "CAN.h"
#include "vec3.h"
#include "EEPROM.h"
#include "Wire.h"
#include "SPI.h"
#include "I2Cdev.h"
#include "MPU60X0.h"
#include "RazorIMU.h"
#include "MotionerIMU.h"


/// Our serial baudrate
static const long BAUDRATE = 57600;

/// MotionerIMU object
MotionerIMU motioner;


/// setup
void setup()
{
  /// Use serial for get some information from MotionerIMU
  Serial.begin(BAUDRATE);
    
  Serial.println("[Motioner IMU]");
    
  /// Use I2C
  Wire.begin();
  
  /// Setup MotionerIMU
  motioner.setup();
}


/// main loop
void loop()
{
  /// Update MotionerIMU
  motioner.update();
}


