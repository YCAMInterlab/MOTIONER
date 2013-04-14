///
/// MotionerLED and Serial debug
///
/// Developed by Onishi Yoshito
///
/// Copyright Onishi Yoshito and YCAM InterLab all rights reserved
/// This Arduino sketch is published under the __________
/// 

#include "SPI.h"
#include "CAN.h"

#include "CANLoopBackTest.h"
CANLoopBackTest canLoopBackTest;

static const long BAUDRATE = 57600;

void setup()
{
  Serial.begin(BAUDRATE);
    
  Serial.println("-- MotionerIMU CAN loopback test --"); 
  canLoopBackTest.setup();
}


void loop()
{
  canLoopBackTest.update();
}
