// I2Cdev library collection - MPU60X0 I2C device class
// Based on InvenSense MPU-6050 register map document rev. 2.0, 5/19/2011 (RM-MPU-6000A-00)
// 10/3/2011 by Jeff Rowberg <jeff@rowberg.net>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// Changelog:
//     ... - ongoing debug release

// NOTE: THIS IS ONLY A PARIAL RELEASE. THIS DEVICE CLASS IS CURRENTLY UNDERGOING ACTIVE
// DEVELOPMENT AND IS STILL MISSING SOME IMPORTANT FEATURES. PLEASE KEEP THIS IN MIND IF
// YOU DECIDE TO USE THIS PARTICULAR CODE FOR ANYTHING.

/* ============================================
 I2Cdev device library code is placed under the MIT license
 Copyright (c) 2011 Jeff Rowberg
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ===============================================
 */

#ifndef _MPU60X0_H_
#define _MPU60X0_H_

#include <avr/pgmspace.h>

#define MPU60X0_ADDRESS_AD0_LOW     0x68 // address pin low (GND), default for InvenSenseevaluation board
#define MPU60X0_ADDRESS_AD0_LOW     0x68 // address pin low (GND), default for InvenSense evaluation board
#define MPU60X0_DEFAULT_ADDRESS     MPU60X0_ADDRESS_AD0_LOW
#define MPU60X0_RA_GYRO_CONFIG      0x1B
#define MPU60X0_RA_ACCEL_CONFIG     0x1C
#define MPU60X0_RA_INT_PIN_CFG      0x37
#define MPU60X0_RA_ACCEL_XOUT_H     0x3B
#define MPU60X0_RA_USER_CTRL        0x6A
#define MPU60X0_RA_PWR_MGMT_1       0x6B
#define MPU60X0_RA_WHO_AM_I         0x75
#define MPU60X0_GYRO_FS_2000        0x03
#define MPU60X0_GCONFIG_FS_SEL_BIT      4
#define MPU60X0_GCONFIG_FS_SEL_LENGTH   2
#define MPU60X0_GYRO_FS_250         0x00
#define MPU60X0_ACONFIG_AFS_SEL_BIT         4
#define MPU60X0_ACONFIG_AFS_SEL_LENGTH      2
#define MPU60X0_ACCEL_FS_2          0x00
#define MPU60X0_INTCFG_I2C_BYPASS_EN_BIT    1
// TODO: figure out what these actually do UMPL source code is not very obivous
#define MPU60X0_USERCTRL_I2C_MST_EN_BIT         5
#define MPU60X0_PWR1_DEVICE_RESET_BIT   7
#define MPU60X0_PWR1_SLEEP_BIT          6
#define MPU60X0_PWR1_CLKSEL_BIT         2
#define MPU60X0_PWR1_CLKSEL_LENGTH      3
#define MPU60X0_CLOCK_PLL_XGYRO         0x01
#define MPU60X0_WHO_AM_I_BIT        6
#define MPU60X0_WHO_AM_I_LENGTH     6


// note: DMP code memory blocks defined at end of header file

class MPU60X0 {
public:
    MPU60X0();
    MPU60X0(uint8_t address);
    
    void initialize();
    bool testConnection();
    void setFullScaleGyroRange(uint8_t range);
    void setFullScaleAccelRange(uint8_t range);
    void setI2CBypassEnabled(bool enabled);
    void setI2CMasterModeEnabled(bool enabled);
    void reset();
    void setSleepEnabled(bool enabled);
    void setClockSource(uint8_t source);
    uint8_t getDeviceID();
    void getMotion6(int16_t* ax, int16_t* ay, int16_t* az, int16_t* gx, int16_t* gy, int16_t* gz);
    
private:
    uint8_t devAddr;
    uint8_t buffer[14];
    
};


#endif /* _MPU60X0_H_ */
