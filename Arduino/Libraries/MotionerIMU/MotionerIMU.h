//
//  MotionerIMU.h
//  MotionerIMU
//
//  Created by Onishi Yoshito on 12/1/12.
//  Copyright (c) 2012 Onishi Yoshito. All rights reserved.
//

#ifndef __MotionerIMU__MotionerIMU__
#define __MotionerIMU__MotionerIMU__


//#define DEBUG
#include "RazorIMU.h"

#include "vec3.h"
#include "Quaternion.h"

#include <Wire.h>

#include <EEPROM.h>

/// CAN
#include <CAN.h>
#include <SPI.h>


class MotionerIMU
{
public:
    //enum Joints {
    //    JOINT_HIPS              = 0, // a
    //    JOINT_ABDOMEN           = 1, // b
    //    JOINT_CHEST             = 2, // c
    //    JOINT_NECK              = 3, // d
    //    
    //    JOINT_LEFT_HIP          = 4, // e
    //    JOINT_LEFT_KNEE         = 5, // f
    //    JOINT_LEFT_ANKLE        = 6, // g
    //    
    //    JOINT_RIGHT_HIP         = 7, // h
    //    JOINT_RIGHT_KNEE        = 8, // i
    //    JOINT_RIGHT_ANKLE       = 9, // j
    //    
    //    JOINT_LEFT_COLLAR       = 10, // k
    //    JOINT_LEFT_SHOULDER     = 11, // l
    //    JOINT_LEFT_ELBOW        = 12, // m
    //    JOINT_LEFT_WRIST        = 13, // n
    //    
    //    JOINT_RIGHT_COLLAR      = 14, // o
    //    JOINT_RIGHT_SHOULDER    = 15, // p
    //    JOINT_RIGHT_ELBOW       = 16, // q
    //    JOINT_RIGHT_WRIST       = 17, // r
    //    
    //    NUM_JOINTS              = 18,
    //};
    
public:
    MotionerIMU();
    
    /// main
    void setup();
    void update();

    /// CAN
    void updateCAN();
    uint8_t getCANControlMessage();
    void sendCANDataQuaternion();
    void sendCANDataAux();
    
    /// Serial
    void updateSerial();
    void writeNodeId();
    char serialBusyWait();
    
    /// MotionerIMU
    void updateValues();
    
    /// velocity
    void resetVelocityPosition();
    bool updateVelocity();
    
public:
    /// debug LED
    /// default motioner baud 
    static const uint8_t LED;
    
private:
    static const int FREEIMU_EEPROM_SIZE;
    
    static const int EEPROM_SIGNATURE;
    static const int EEPROM_BASE;
    static const int EEPROM_NODE_ID;
    
    /// CAN
    static const int BUS_SPEED;
    
    static const int FLOAT_SIZE;
    
private:
    union floatAndByte {
        float f;
        uint8_t b[4];
    };
    
    struct vec4 { float x, y, z, w; };
    
private:
    /// RazorIMU
    RazorIMU mRazorIMU;
        
    /// MotionerIMU
    uint8_t mNodeId;
    
    /// velocity
    vec3 acc, accrawprev, acclp, accprev, vel;
    float totalSpeed;
    const float lerpRate;
    const float invLerpRate;
    
    float mRawAcce[3];
    /// velocity
};

#endif /* defined(__MotionerIMU__MotionerIMU__) */
