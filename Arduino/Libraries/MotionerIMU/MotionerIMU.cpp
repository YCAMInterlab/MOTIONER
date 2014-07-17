//
//  MotionerIMU.cpp
//  MotionerIMU
//
//  Created by Onishi Yoshito on 12/1/12.
//  Copyright (c) 2012 Onishi Yoshito. All rights reserved.
//

#include "MotionerIMU.h"

/// ------------------------------------------------ ///
const uint8_t MotionerIMU::LED = 9;

const int MotionerIMU::FREEIMU_EEPROM_SIZE = sizeof(float) * 6 + sizeof(int) * 6;

const int MotionerIMU::EEPROM_SIGNATURE = 0x24;
const int MotionerIMU::EEPROM_BASE = 0x40; // 64
const int MotionerIMU::EEPROM_NODE_ID = EEPROM_BASE+1;

const int MotionerIMU::BUS_SPEED = 1000;

const int MotionerIMU::FLOAT_SIZE = 4;

/// ------------------------------------------------ ///
static void blink(uint8_t pin, uint8_t times, uint16_t ms);

/// ------------------------------------------------ ///
MotionerIMU::MotionerIMU() :
totalSpeed(0),
lerpRate(0.01),
invLerpRate(1. - lerpRate), /// velocity
mNodeId(0),
mOutPutSerial(false)
{
}

/// -------------------- Setup -------------------- ///
void MotionerIMU::setup()
{
    /// Setup debug LED
    pinMode(LED, OUTPUT);
    
    Serial.println("setup Motioner IMU Node");
    
    /// RazorIMU
    /// initialize razor imu
    mRazorIMU.setup();
    
    Serial.println("init CAN(MCP2515)");
    //CAN
    //Set to determine whether uploading transmitter or receiver code
    //canDebugMode = TX;
    
    /// Initialize CAN bus class
    /// This class initializes SPI communications with MCP2515
    CAN.begin();
    //Configuration mode
    uint8_t addrHi, addrLo;
    
    /// Software reset
    CAN.reset();
    /// Set CAN Bus speed as 1Mbps
    CAN.baudConfig(BUS_SPEED);
    
    /// FIXME: should set mask for performance
    //CAN.setMask(MASK_SID_ALL_HIT);//test mask all
    //if (canDebugMode == TX)
    //CAN.setMask(MASK_SID_CPL_MATCH);//test no mask
    
    //CAN.setMask(0x0040);//test no mask
    
    //delay(100);
    //
    //CAN.readRegister(RXM0SIDH, &addrHi);
    //CAN.readRegister(RXM0SIDL, &addrLo);
    //
    //Serial.print(addrHi, HEX);
    //Serial.println(addrLo, HEX);
    //while(1);
    
    /// Normal mode
    CAN.setMode(CAN_MODE_NORMAL);
    
    Serial.println("success");
    
    /// MotionerIMU
    Serial.println("loading Node ID");
    
    mNodeId = EEPROM.read(EEPROM_NODE_ID);
    Serial.print("NodeID=");
    Serial.print(mNodeId);
    Serial.println("");
    
    /// tell who am i
    if (mNodeId < 255) {
        if (mNodeId>=10)
            blink(LED, mNodeId/10, 500);
        blink(LED, mNodeId%10, 100);
    }
    
    resetVelocityPosition();
    
    Serial.println("setup finished!");
}

/// -------------------- Update -------------------- ///
void MotionerIMU::update()
{
    updateSerial();
    
    if (mOutPutSerial) outPutSerial();
    
    updateValues();
    
    const bool ret = updateVelocity();
    
    updateCAN();
}

/// -------------------- CAN -------------------- ///
void MotionerIMU::updateCAN()
{
    switch (getCANControlMessage()) {
            
        case 0: break; /// no message
            
        case 'q':
            //Serial.println("StartTransfer");
            digitalWrite(LED, HIGH);
            sendCANDataQuaternion();
            sendCANDataAux();
            digitalWrite(LED, LOW);
            break;
            
        default: break;
    }
}

uint8_t MotionerIMU::getCANControlMessage()
{
    //CAN
    uint8_t rxStatus = 0;
    unsigned short frameId = 0x0000;
    uint8_t frameData[8];
    uint8_t length = 0;
    uint8_t command = 0;
    
    for (int i=0; i<8; i++)
        frameData[i] = 0x00;
    
    rxStatus = CAN.readStatus();
    
    if ((rxStatus & 0x01) == 0x01)
        CAN.readDATA_ff_0(&length,frameData,&frameId);
    else if ((rxStatus & 0x02) == 0x02)
        CAN.readDATA_ff_1(&length,frameData,&frameId);
    
    if (((rxStatus & 0x01) == 0x01) || ((rxStatus & 0x02) == 0x02)) {
        
        /// debug print can message
        //Serial.print(rxStatus, HEX);
        //Serial.print("/");
        //Serial.print(length, HEX);
        //Serial.print("/");
        //Serial.print(frameId, HEX);
        //Serial.print("/");
        //for (int i=0;i<length;i++)
        //    if (i%2==0 && frameData[i] != 0xFF)
        //        Serial.print(frameData[i], HEX);
        
        const uint8_t nodeId = frameId >> 3;
        //const uint8_t slot = frameId & 0x01;
        
        if (length >= 2) {
            /// 0x40 -> global message
            /// 0x0040+n -> to a node
            if(nodeId == 0x0040 || nodeId == 0x0040+mNodeId) {
                uint8_t c1 = frameData[0];
                uint8_t c2 = frameData[1];
                
                /// motioner control message
                if (c1 == 'm' && c2 == 't') {
                    command = frameData[2];
                    //Serial.print("/");
                    //Serial.print(command, HEX);
                }
            }
        }
        
        //Serial.print(frameData[0],HEX);
        //Serial.print("\n");
    }
    
    return command;
}

void MotionerIMU::sendCANDataQuaternion()
{
    /// 0x0 - 0x7FF (11bits)
    const unsigned short nodeId = mNodeId;
    unsigned short frameId = 0; // 0x[nodeId][nodeId][reserve][RXSlotId]
    const uint8_t length = 8;
    uint8_t frameData[length];

    uint8_t *b = (uint8_t *)&mRazorIMU.getQuaternion().x;
    //uint8_t *b = (uint8_t *)&mYPR.x;
    
    /// Send to CAN
    for (int i=0; i<2; i++) {
        
        for (int j=0; j<length; j++) {
            frameData[j] = b[j+i*length];
        }
        
        /// Switch buffer
        switch (i) {
            case 0:
                /// frameId (11bits)
                frameId = (nodeId << 3) + 0x0000; /// 1st data
                CAN.load_ff_0(length,frameId,frameData);
                CAN.send_0();
                /// implement continue; when failed to send data
                break;
            case 1:
                /// frameId (11bits)
                frameId = (nodeId << 3) + 0x0001; /// 2nd data
                CAN.load_ff_1(length,frameId,frameData);
                CAN.send_1();
                /// implement continue; when failed to send data
                break;
        }
    }
}

void MotionerIMU::sendCANDataAux()
{
    //Serial.print("v");
    
    /// 0x0 - 0x7FF (11bits)
    const unsigned short nodeId = mNodeId;
    unsigned short frameId = 0; // 0x[nodeId][nodeId][reserve][RXSlotId]
    const uint8_t length = FLOAT_SIZE*2;
    uint8_t frameData[length];
    
    floatAndByte outVel, bearing;
    
    //outVel = totalSpeed;
    outVel.f = vel.y;
    bearing.f = 12345.0f;
    
    for (int i=0; i<FLOAT_SIZE; i++) {
        frameData[i] = outVel.b[i];
        //Serial.println(vel.b[i]);
        frameData[i+FLOAT_SIZE] = bearing.b[i];
    }
    
    frameId = (nodeId << 3) + 0x0002; /// 3rd data
    
    CAN.load_ff_2(length, frameId, frameData);
    CAN.send_2();
}

/// -------------------- Velocity -------------------- ///
void MotionerIMU::resetVelocityPosition() {
    acclp.zero();
    accprev.zero();
    vel.zero();
}

bool MotionerIMU::updateVelocity() {
    
    mRazorIMU.getRawAcce(mRawAcce);
    
    // get acceleration
    acc.set(&mRawAcce[0]);
    
    if(acc != accrawprev) {
        // copy acc for checking on next frame
        accrawprev.set(acc);
        
        // get orientation quaternion
        //mFreeIMU.getQ(values, mQuaternion);
        
        /// sorry, not implemented yet.......
        // compensate for gravity given orientation
        //mFreeIMU.gravityCompensateAcc(acc, mQuaternion);
        
        // low pass acceleration
        acclp *= invLerpRate;
        acclp += (acc * lerpRate);
        
        // high pass acceleration
        acc -= acclp;
        
        // integrate to get velocity
        vel += acc;
        
        // integrate trapezoidal remainder
        acc -= accprev;
        acc /= 2;
        vel += acc;
        
        accprev.set(acc);
        
        totalSpeed = vel.length();
        
        return true;
    }
    return false;
}

/// -------------------- Serial -------------------- ///
void MotionerIMU::updateSerial()
{
    // Read incoming control messages
    if (Serial.available() >= 2) {
        const int control = Serial.read();
        const int command = Serial.read(); // Commands
        
        // Start of new RazorIMU control message
        if (control == '#') {
            /// read serial again internally
            mRazorIMU.updateSerial(command);
        }
        else if (control == '@') {
            if (command == 'n') {
                writeNodeId();
            }
            else if (command == 'q') {
                Serial.print("Quat: ");
                Serial.print(mRazorIMU.getQuaternion().x);
                Serial.print(", ");
                Serial.print(mRazorIMU.getQuaternion().y);
                Serial.print(", ");
                Serial.print(mRazorIMU.getQuaternion().z);
                Serial.print(", ");
                Serial.print(mRazorIMU.getQuaternion().w);
                Serial.println("");
            }
            else if (command == 'o') {
                if (Serial.available() >= 1) {
                    int command2 = Serial.read();
                    if (command2 == '1') {
                        mOutPutSerial = true;
                        Serial.println("Begin output serial data.");
                    }
                    else if (command2 == '0') {
                        mOutPutSerial = false;
                        Serial.println("End output serial data.");
                    }
                    
                }
            }
        }
    }
}

void MotionerIMU::outPutSerial()
{
//    Serial.print(mRazorIMU.getQuaternion().x);
//    Serial.print(",");
//    Serial.print(mRazorIMU.getQuaternion().y);
//    Serial.print(",");
//    Serial.print(mRazorIMU.getQuaternion().z);
//    Serial.print(",");
//    Serial.print(mRazorIMU.getQuaternion().w);
//    Serial.print(",");
    Serial.print(vel.x);
    Serial.print(",");
    Serial.print(vel.y);
    Serial.print(",");
    Serial.print(vel.z);
    Serial.print(",");
    Serial.print(acc.length());
    Serial.println("");
}

char MotionerIMU::serialBusyWait()
{
    while(!Serial.available()) {
        ; // do nothing until ready
    }
    return Serial.read();
}

/// ---------------------- IMU ---------------------- ///
void MotionerIMU::updateValues()
{
    mRazorIMU.updateAHRS();
    //mQuaternion = mRazorIMU.getQuaternion();
    //mYPR.x = mRazorIMU.getYaw();
    //mYPR.y = mRazorIMU.getPitch();
    //mYPR.z = mRazorIMU.getRoll();
    //mYPR.w = 1.0f;
}


/// -------------------- EEPROM -------------------- ///
/// using serial busy wait and callback
void MotionerIMU::writeNodeId()
{
    digitalWrite(LED, HIGH);
    Serial.println("Write Node ID on EEPROM. Enter Node ID");
    uint8_t nodeId = serialBusyWait()-97; // keyboard 'a' to 'z'
    EEPROM.write(EEPROM_BASE, EEPROM_SIGNATURE);
    EEPROM.write(EEPROM_NODE_ID, nodeId);
    mNodeId = nodeId;
    Serial.print("Successfully wrote Node Id: ");
    Serial.print(nodeId);
    Serial.println("");
    digitalWrite(LED, LOW);
}

void blink(uint8_t pin, uint8_t times, uint16_t ms)
{
    for (uint8_t i=0; i<times; i++) {
        digitalWrite(pin, HIGH);
        delay(ms >> 1);
        digitalWrite(pin, LOW);
        delay(ms >> 1);
    }
}


