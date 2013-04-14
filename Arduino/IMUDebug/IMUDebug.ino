#include "CAN.h"
#include "vec3.h"
#include "EEPROM.h"
#include "Wire.h"
#include "SPI.h"
#include "I2Cdev.h"
#include "MPU60X0.h"
#include "RazorIMU.h"

RazorIMU razor;


void setup()
{
  Serial.begin(57600);
  Serial.println("-- extended RazorAHRS debug --");
  razor.setup();
}

void loop()
{
  // Read incoming control messages
  if (Serial.available() >= 2) {
      // Start of new RazorIMU control message
      if (Serial.read() == '#') {
          /// read serial again internally
          razor.updateSerial(Serial.read());
      }
  }
  razor.updateAHRS();
}
