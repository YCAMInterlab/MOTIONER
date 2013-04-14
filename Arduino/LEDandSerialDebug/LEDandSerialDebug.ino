///
/// MotionerLED and Serial debug
///
/// Developed by Onishi Yoshito
///
/// Copyright Onishi Yoshito and YCAM InterLab all rights reserved
/// This Arduino sketch is published under the __________
/// 

static const long BAUDRATE = 57600;
static const int MOTIONER_LED = 9;

void blink(uint8_t pin, uint8_t times, uint16_t ms);

void setup()
{
  Serial.begin(BAUDRATE);
  Serial.println("-- MotionerIMU LED and serial debug --"); 
}

void loop()
{
  blink(MOTIONER_LED, 3, 50);
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
