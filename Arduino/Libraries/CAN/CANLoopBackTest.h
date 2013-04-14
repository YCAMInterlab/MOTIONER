//
//  CANLoopBackTest.h
//  Motioner
//
//  Created by Onishi Yoshito on 1/20/13.
//  Copyright (c) 2013 Onishi Yoshito. All rights reserved.
//

#ifndef Motioner_CANLoopBackTest_h
#define Motioner_CANLoopBackTest_h

//Code by Patrick Cruce(pcruce_at_igpp.ucla.edu)
//Uses CAN extended library, designed for Arduino with 16 Mhz oscillator
//Library found at: http://code.google.com/p/canduino/source/browse/trunk/#trunk%2FLibrary%2FCAN
//This runs a simple test of the hardware with the MCP 2515 CAN Controller in loopback mode.
//If using over physical bus rather than loopback, and you have high bus
//utilization, you'll want to turn the baud of the serial port up or log
//to SD card, because frame drops can occur due to the reader code being
//occupied with writing to the port.
//In our testing over a 40 foot cable, we didn't have any problems with
//synchronization or frame drops due to SPI,controller, or propagation delays
//even at 1 Megabit.  But we didn't do any tests that required arbitration
//with multiple nodes.

#include "CAN.h"
#include <SPI.h>

struct CANLoopBackTest {
    //global variable used to determine whether loop should
    //be in Tx or Rx mode.
    int state;
    //global variable used to toggle pin
    int pin;
    static const int BUS_SPEED = 1000;
    
    void setup() {
        
        
        //Serial.begin(115200);
        
        pinMode(7,OUTPUT);
        pinMode(8,OUTPUT);
        
        // initialize CAN bus class
        // this class initializes SPI communications with MCP2515
        CAN.begin();
        CAN.baudConfig(BUS_SPEED);
        CAN.setMode(CAN_MODE_LOOPBACK);  // set to "NORMAL" for standard com
        
        //Wait 10 seconds so that I can still upload even
        //if the previous iteration spams the serial port
        //delay(10000);
        
        state = 0;
        
        pin = LOW;
    }
    
    void update() {
        
        byte length,rx_status,i;
        unsigned short frame_id;
        byte frame_data[8];
        
        //signal iterations on pin 8
        digitalWrite(8,pin);
        
        switch(state) {
            case 0: //Tx
                
                //signal state on pin 7;
                digitalWrite(7,LOW);
                
                frame_data[0] = 0x55;
                frame_data[1] = 0xAA;
                frame_data[2] = 0x00;
                frame_data[3] = 0xFF;
                frame_data[4] = 0x1A;
                frame_data[5] = 0xCF;
                frame_data[6] = 0xFC;
                frame_data[7] = 0x1D;
                
                frame_id = 0x05A5;
                
                length = 8;
                
                CAN.load_ff_0(length,frame_id,frame_data);
                CAN.send_0();
                
                state = 1;
                
                break;
            case 1: //Rx
                
                //signal state on pin 7
                digitalWrite(7,HIGH);
                //clear receive buffers, just in case.
                frame_data[0] = 0x00;
                frame_data[1] = 0x00;
                frame_data[2] = 0x00;
                frame_data[3] = 0x00;
                frame_data[4] = 0x00;
                frame_data[5] = 0x00;
                frame_data[6] = 0x00;
                frame_data[7] = 0x00;
                
                frame_id = 0x0000;
                
                length = 0;
                
                rx_status = CAN.readStatus();
                
                if (rx_status & 0x80 == 0x80) {
                    
                    CAN.readDATA_ff_0(&length,frame_data,&frame_id);
                    state = 0;
                    
                } else if (rx_status & 0x40 == 0x40) {
                    
                    CAN.readDATA_ff_1(&length,frame_data,&frame_id);
                    state = 0;
                    
                }
                
                Serial.print("Rx: ");
                Serial.print(rx_status,HEX);
                Serial.print(" / ");
                Serial.print(length,HEX);
                
                Serial.print(" / ");
                Serial.print(frame_id,HEX);
                
                for (i=0;i<8;i++) {
                    Serial.print(" / ");
                    Serial.print(frame_data[i],HEX);
                }
                
                Serial.println();
                break;      
        }
        
        if (pin == LOW) {
            pin = HIGH;
        } else {
            pin = LOW;
        }  
        
        delay(500);
        
    }

};






#endif
