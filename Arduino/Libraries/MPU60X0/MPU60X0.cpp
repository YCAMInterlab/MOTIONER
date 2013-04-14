// I2Cdev library collection - MPU60X0 I2C device class
// Based on InvenSense MPU-6050 register map document rev. 2.0, 5/19/2011 (RM-MPU-6000A-00)
// 8/24/2011 by Jeff Rowberg <jeff@rowberg.net>
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

#include "MPU60X0.h"
#include "I2Cdev.h"

/** Default constructor, uses default I2C address or default SS Pin if SPI
 * @see MPU60X0_DEFAULT_ADDRESS
 * @see MPU60X0_DEFAULT_SS_PIN
 */
MPU60X0::MPU60X0() {

}

/** Specific address constructor.
 * @param address I2C address or Slave select Pin (SPI)
 * @see MPU60X0_DEFAULT_ADDRESS
 * @see MPU60X0_DEFAULT_SS_PIN
 * @see MPU60X0_ADDRESS_AD0_LOW
 * @see MPU60X0_ADDRESS_AD0_HIGH
 */
MPU60X0::MPU60X0(uint8_t address) {
    devAddr = address;
}

/** Power on and prepare for general usage.
 * This will activate the device and take it out of sleep mode (which must be done
 * after start-up). This function also sets both the accelerometer and the gyroscope
 * to their most sensitive settings, namely +/- 2g and +/- 250 degrees/sec, and sets
 * the clock source to use the X Gyro for reference, which is slightly better than
 * the default internal clock source.
 */
void MPU60X0::initialize() {
    setClockSource(MPU60X0_CLOCK_PLL_XGYRO);
    setFullScaleGyroRange(MPU60X0_GYRO_FS_250);
    setFullScaleAccelRange(MPU60X0_ACCEL_FS_2);
    setSleepEnabled(false); // thanks to Jack Elston for pointing this one out!
}

/** Verify the I2C/SPI connection.
 * Make sure the device is connected and responds as expected.
 * @return True if connection is valid, false otherwise
 */
bool MPU60X0::testConnection() {
    return getDeviceID() == 0b110100;
}

/** Set full-scale gyroscope range.
 * @param range New full-scale gyroscope range value
 * @see getFullScaleRange()
 * @see MPU60X0_GYRO_FS_250
 * @see MPU60X0_RA_GYRO_CONFIG
 * @see MPU60X0_GCONFIG_FS_SEL_BIT
 * @see MPU60X0_GCONFIG_FS_SEL_LENGTH
 */
void MPU60X0::setFullScaleGyroRange(uint8_t range) {
    I2Cdev::writeBits(false, devAddr, MPU60X0_RA_GYRO_CONFIG, MPU60X0_GCONFIG_FS_SEL_BIT, MPU60X0_GCONFIG_FS_SEL_LENGTH, range);
}

/** Set full-scale accelerometer range.
 * @param range New full-scale accelerometer range setting
 * @see getFullScaleAccelRange()
 */
void MPU60X0::setFullScaleAccelRange(uint8_t range) {
    I2Cdev::writeBits(false, devAddr, MPU60X0_RA_ACCEL_CONFIG, MPU60X0_ACONFIG_AFS_SEL_BIT, MPU60X0_ACONFIG_AFS_SEL_LENGTH, range);
}

/** Set I2C bypass enabled status.
 * When this bit is equal to 1 and I2C_MST_EN (Register 106 bit[5]) is equal to
 * 0, the host application processor will be able to directly access the
 * auxiliary I2C bus of the MPU-60X0. When this bit is equal to 0, the host
 * application processor will not be able to directly access the auxiliary I2C
 * bus of the MPU-60X0 regardless of the state of I2C_MST_EN (Register 106
 * bit[5]).
 * @param enabled New I2C bypass enabled status
 * @see MPU60X0_RA_INT_PIN_CFG
 * @see MPU60X0_INTCFG_I2C_BYPASS_EN_BIT
 */
void MPU60X0::setI2CBypassEnabled(bool enabled) {
    I2Cdev::writeBit(false, devAddr, MPU60X0_RA_INT_PIN_CFG, MPU60X0_INTCFG_I2C_BYPASS_EN_BIT, enabled);
}

/** Set I2C Master Mode enabled status.
 * @param enabled New I2C Master Mode enabled status
 * @see getI2CMasterModeEnabled()
 * @see MPU60X0_RA_USER_CTRL
 * @see MPU60X0_USERCTRL_I2C_MST_EN_BIT
 */
void MPU60X0::setI2CMasterModeEnabled(bool enabled) {
    I2Cdev::writeBit(false, devAddr, MPU60X0_RA_USER_CTRL, MPU60X0_USERCTRL_I2C_MST_EN_BIT, enabled);
}

// PWR_MGMT_1 register

/** Trigger a full device reset.
 * A small delay of ~50ms may be desirable after triggering a reset.
 * @see MPU60X0_RA_PWR_MGMT_1
 * @see MPU60X0_PWR1_DEVICE_RESET_BIT
 */
void MPU60X0::reset() {
    I2Cdev::writeBit(false, devAddr, MPU60X0_RA_PWR_MGMT_1, MPU60X0_PWR1_DEVICE_RESET_BIT, true);
}

/** Set sleep mode status.
 * @param enabled New sleep mode enabled status
 * @see getSleepEnabled()
 * @see MPU60X0_RA_PWR_MGMT_1
 * @see MPU60X0_PWR1_SLEEP_BIT
 */
void MPU60X0::setSleepEnabled(bool enabled) {
    I2Cdev::writeBit(false, devAddr, MPU60X0_RA_PWR_MGMT_1, MPU60X0_PWR1_SLEEP_BIT, enabled);
}

/** Set clock source setting.
 * An internal 8MHz oscillator, gyroscope based clock, or external sources can
 * be selected as the MPU-60X0 clock source. When the internal 8 MHz oscillator
 * or an external source is chosen as the clock source, the MPU-60X0 can operate
 * in low power modes with the gyroscopes disabled.
 *
 * Upon power up, the MPU-60X0 clock source defaults to the internal oscillator.
 * However, it is highly recommended that the device be configured to use one of
 * the gyroscopes (or an external clock source) as the clock reference for
 * improved stability. The clock source can be selected according to the following table:
 *
 * <pre>
 * CLK_SEL | Clock Source
 * --------+--------------------------------------
 * 0       | Internal oscillator
 * 1       | PLL with X Gyro reference
 * 2       | PLL with Y Gyro reference
 * 3       | PLL with Z Gyro reference
 * 4       | PLL with external 32.768kHz reference
 * 5       | PLL with external 19.2MHz reference
 * 6       | Reserved
 * 7       | Stops the clock and keeps the timing generator in reset
 * </pre>
 *
 * @param source New clock source setting
 * @see getClockSource()
 * @see MPU60X0_RA_PWR_MGMT_1
 * @see MPU60X0_PWR1_CLKSEL_BIT
 * @see MPU60X0_PWR1_CLKSEL_LENGTH
 */
void MPU60X0::setClockSource(uint8_t source) {
    I2Cdev::writeBits(false, devAddr, MPU60X0_RA_PWR_MGMT_1, MPU60X0_PWR1_CLKSEL_BIT, MPU60X0_PWR1_CLKSEL_LENGTH, source);
}

// WHO_AM_I register

/** Get Device ID.
 * This register is used to verify the identity of the device (0b110100).
 * @return Device ID (should be 0x68, 104 dec, 150 oct)
 * @see MPU60X0_RA_WHO_AM_I
 * @see MPU60X0_WHO_AM_I_BIT
 * @see MPU60X0_WHO_AM_I_LENGTH
 */
uint8_t MPU60X0::getDeviceID() {
    I2Cdev::readBits(false, devAddr, MPU60X0_RA_WHO_AM_I, MPU60X0_WHO_AM_I_BIT, MPU60X0_WHO_AM_I_LENGTH, buffer);
    return buffer[0];
}

/** Get raw 6-axis motion sensor readings (accel/gyro).
 * Retrieves all currently available motion sensor values.
 * @param ax 16-bit signed integer container for accelerometer X-axis value
 * @param ay 16-bit signed integer container for accelerometer Y-axis value
 * @param az 16-bit signed integer container for accelerometer Z-axis value
 * @param gx 16-bit signed integer container for gyroscope X-axis value
 * @param gy 16-bit signed integer container for gyroscope Y-axis value
 * @param gz 16-bit signed integer container for gyroscope Z-axis value
 * @see getAcceleration()
 * @see getRotation()
 * @see MPU60X0_RA_ACCEL_XOUT_H
 */
void MPU60X0::getMotion6(int16_t* ax, int16_t* ay, int16_t* az, int16_t* gx, int16_t* gy, int16_t* gz) {
    I2Cdev::readBytes(false, devAddr, MPU60X0_RA_ACCEL_XOUT_H, 14, buffer);
    *ax = (((int16_t)buffer[0]) << 8) | buffer[1];
    *ay = (((int16_t)buffer[2]) << 8) | buffer[3];
    *az = (((int16_t)buffer[4]) << 8) | buffer[5];
    *gx = (((int16_t)buffer[8]) << 8) | buffer[9];
    *gy = (((int16_t)buffer[10]) << 8) | buffer[11];
    *gz = (((int16_t)buffer[12]) << 8) | buffer[13];
}
