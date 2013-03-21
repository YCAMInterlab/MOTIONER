/******************************************************************************************
 * Gyroscope calibration for MotionerIMU 
 * 9 Degree of Measurement Attitude and Heading Reference System for MOTIONER which is 
 * opensource intertial motion capture system 
 *
 * Released under GNU GPL (General Public License) v3.0
 * Copyright (C) 2013-2014 Yoshito Onishi
 * Written by Yoshito Onishi (https://github.com/YoshitoONISHI)
 *
 * Infos, updates, bug reports and feedback:
 *    https://github.com/YCAMInterlab/Motioner
 *
 * This sketch is based on Magnetometer Sampling Sketch for Razor AHRS v1.4.1 which
 * written by Peter Bartz (peter-bartz@gmx.de)
 * Also see below license
 *
 ****************************************************************************************** 
 *
 * Magnetometer Sampling Sketch for Razor AHRS v1.4.1
 * 9 Degree of Measurement Attitude and Heading Reference System
 * for Sparkfun "9DOF Razor IMU" and "9DOF Sensor Stick"
 *
 * Released under GNU GPL (General Public License) v3.0
 * Copyright (C) 2011-2012 Quality & Usability Lab, Deutsche Telekom Laboratories, TU Berlin
 * Written by Peter Bartz (peter-bartz@gmx.de)
 *
 * Infos, updates, bug reports and feedback:
 *     http://dev.qu.tu-berlin.de/projects/sf-razor-9dof-ahrs
 ******************************************************************************************/

/*
  NOTE: There seems to be a bug with the serial library in the latest Processing
 versions 1.5 and 1.5.1: "WARNING: RXTX Version mismatch ...". The previous version
 1.2.1 works fine and is still available on the web.
 */

/*
  NOTE: You have to install a library, before this sketch can be run!
 We're using EJML for matrix math, because it's really fast:
 http://code.google.com/p/java-matrix-benchmark/
 Also, it's released under LGPL, which fits well with our GPL.
 Get the library from: http://code.google.com/p/efficient-java-matrix-library/ (you only need
 the .jar file), find your Processing "libraries" folder (normally this is Processing/libraries
 in your user documents folder). Create a folder "EJML" inside "libraries",
 create a folder "library" inside "EJML" and put the .jar inside. Rename to EJML.jar. So you
 should have "libraries/EJML/library/EJML.jar". Restart Processing and you're good.
 More info on installing libraries in Processing: http://wiki.processing.org/w/How_to_Install_a_Contributed_Library
 */
import org.ejml.data.*;
import org.ejml.simple.*;
import org.ejml.ops.*;

import java.io.*;

import processing.opengl.*;
import processing.serial.*;

import java.nio.*;

// IF THE SKETCH CRASHES OR HANGS ON STARTUP, MAKE SURE YOU ARE USING THE RIGHT SERIAL PORT:
// 1. Have a look at the Processing console output of this sketch.
// 2. Look for the serial port list and find the port you need (it's the same as in Arduino).
// 3. Set your port number here:
final static int SERIAL_PORT_NUM = 6;
// 4. Try again.

final static int SERIAL_PORT_BAUD_RATE = 57600;

PFont font;
Serial serial;

boolean synched = false;

// Skip incoming serial stream data until token is found
boolean readToken(Serial serial, String token) {
  // Wait until enough bytes are available
  if (serial.available() < token.length())
    return false;

  // Check if incoming bytes match token
  for (int i = 0; i < token.length(); i++) {
    if (serial.read() != token.charAt(i))
      return false;
  }
  return true;
}

// Global setup
void setup() {
  // Setup graphics
  size(800, 800, OPENGL);
  smooth();
  noStroke();
  frameRate(60);
  colorMode(HSB);

  // Load font
  font = loadFont("Univers-66.vlw");
  textFont(font);

  // Setup serial port I/O
  println("AVAILABLE SERIAL PORTS:");
  println(Serial.list());
  String portName = Serial.list()[SERIAL_PORT_NUM];
  println();
  println("HAVE A LOOK AT THE LIST ABOVE AND SET THE RIGHT SERIAL PORT NUMBER IN THE CODE!");
  println("  -> Using port " + SERIAL_PORT_NUM + ": " + portName);
  serial = new Serial(this, portName, SERIAL_PORT_BAUD_RATE);
}

void setupRazor() {
  println("Trying to setup and synch Razor...\n");

  synched = false;
  // On Mac OSX and Linux (Windows too?) the board will do a reset when we connect, which is really bad.
  // See "Automatic (Software) Reset" on http://www.arduino.cc/en/Main/ArduinoBoardProMini
  // So we have to wait until the bootloader is finished and the Razor firmware can receive commands.
  // To prevent this, disconnect/cut/unplug the DTR line going to the board. This also has the advantage,
  // that the angles you receive are stable right from the beginning. 
  delay(3000);  // 3 seconds should be enough

  // Set Razor output parameters
  serial.write("#osrb");  // Turn on binary output of raw sensor data
  serial.write("#o1");    // Turn on continuous streaming output
  serial.write("#oe0");   // Disable error message output

  // Synch with Razor
  serial.clear();  // Clear input buffer up to here
  serial.write("#s00");  // Request synch token
}

float readFloat(Serial s) {
  // Convert from little endian (Razor) to big endian (Java) and interpret as float
  return Float.intBitsToFloat(s.read() + (s.read() << 8) + (s.read() << 16) + (s.read() << 24));
}

boolean bReset = true;

void skipBytes(Serial s, int numBytes) {
  for (int i = 0; i < numBytes; i++) {
    s.read();
  }
}

void reset()
{
  pushMatrix(); 
  {
    translate(width/2, height/2, -900);
    background(0);
    noFill();
    stroke(255);
    sphereDetail(10);
    sphere(400);
    fill(200);
  }
  popMatrix();
  bReset = false;
}

void resynch()
{ 
  synched = false;
  
  delay(1000);  // 1 seconds should be enough
  
  while(serial.available()>0) serial.read();
  
  serial.clear();  // Clear input buffer up to here

  // Set Razor output parameters
  serial.write("#osrb");  // Turn on binary output of raw sensor data
  serial.write("#o1");    // Turn on continuous streaming output
  serial.write("#oe0");   // Disable error message output

  // Synch with Razor
  serial.clear();  // Clear input buffer up to here
  serial.write("#s00");  // Request synch token
}

final int maxGyroSample = 3600;
float[][] gyros = new float[maxGyroSample][3];
int currGyroSample = 0;
float[] gyroAverage = new float[3];

void caliGyro()
{
  if (bReset) {
    reset();
  }

  stroke(0, 0, 0);
  fill(0, 0, 0);
  rect(0, height-200, width, height);
  noStroke();
  fill(200);
  textSize(16);
  text(gyroAverage[0], width/2, height-180, 0);
  text(gyroAverage[1], width/2, height-150, 0);
  text(gyroAverage[2], width/2, height-120, 0);
  textSize(16);
  text("Press 'r' to reset. ", width/2, height-90, 0);
  text("Press SPACE to next calibration.", width/2, height-60, 0);
  text("Press RETURN to write calibration parameters to IMU and go to next calibration.", width/2, height-30, 0);

  pushMatrix(); 
  {
    translate(width/2, height/2, -900);
    // Read and draw new sample
    if (serial.available() >= 36) {
      // Read all available magnetometer data from serial port
      while (serial.available () >= 36) {        

        // Skip acce data
        skipBytes(serial, 12);

        // Skip magn data
        skipBytes(serial, 12);

        gyros[currGyroSample][0] = readFloat(serial);  // x
        gyros[currGyroSample][1] = readFloat(serial);  // y
        gyros[currGyroSample][2] = readFloat(serial);  // z

        if (currGyroSample < maxGyroSample-1) {
          currGyroSample++;
        }

        if (currGyroSample>0) {
          for (int i=0; i<3; i++) {
            gyroAverage[i] = 0;
          }

          for (int j=0; j<currGyroSample; j++) {
            for (int i=0; i<3; i++) {
              gyroAverage[i] += gyros[j][i];
            }
          }

          for (int i=0; i<3; i++) {
            gyroAverage[i] /= currGyroSample;
          }
        }
      }
    }

    // Draw new point

    noStroke();
    if (currGyroSample>0) {
      fill((gyros[currGyroSample-1][0] + 800) / 8, 255, 255);
      translate(gyros[currGyroSample-1][0], gyros[currGyroSample-1][1], gyros[currGyroSample-1][2]);
    }
    sphere(5);
  }
  popMatrix();
}

void draw() {
  // Reset scene
  lights();

  // Sync with Razor 
  if (!synched && frameCount<120) {
    background(0);
    textAlign(CENTER);
    fill(255);
    text("Connecting to Razor...", width/2, height/2, -200);

    if (frameCount == 2) {
      setupRazor();  // Set ouput params and request synch token
    }
    else if (frameCount > 2) {
      synched = readToken(serial, "#SYNCH00\r\n");  // Look for synch token
    }
    return;
  }

  caliGyro();
}

void keyPressed() {
  switch (key) {
  case '0':  // Turn Razor's continuous output stream off
    serial.write("#o0");
    break;
  case '1':  // Turn Razor's continuous output stream on
    serial.write("#o1");
    break;
  case 'f':  // Request one single yaw/pitch/roll frame from Razor (use when continuous streaming is off)
    serial.write("#f");
    break;
    // Calculate and output calibration parameters, output binary magnetometer samples file, quit
  case ENTER:
  case RETURN:
    outputGyroCalibration();
    break;

  case 'r':  // Reset samples and clear screen
    gyros = new float[maxGyroSample][3];
    currGyroSample = 0;
    bReset = true;
    break;
  }
}

/// float -> byte conversion
void printInteger(byte[] data, int offset, int value) throws IllegalArgumentException {
  int size=Integer.SIZE/Byte.SIZE;
  if (data==null||data.length<size||offset<0||data.length-size<offset)

    throw new IllegalArgumentException("Bat Param");
  else {
    for (int i=0;i<size;i++)

      data[offset+i]=Integer.valueOf(value>>(Byte.SIZE*(size-1-i))).byteValue();
  }
}

void printFloat(byte[] data, int offset, float value) throws IllegalArgumentException {
  printInteger(data, offset, Float.floatToIntBits(value));
}

void outputGyroCalibration() 
{
  /// motioner
  byte[] data = new byte[4*3]; /// float *3

  for (int i=0; i<3; i++) {
    printFloat(data, i*4, gyroAverage[i]);
  }

  //serial.clear();  // Clear input buffer up to here
  serial.write("#cg");  // Will send calibrated data to IMU

  /// Write all binary data to eeprom
  for (int i=0; i<3; i++) {
    for (int j=0; j<4; j++) {
      serial.write(data[i*4+3-j]); /// Big endian to little endian
    }
  } 

  //delay(100);

  serial.write("#cw");  // Save calibrated data to Arduino EEPROM

  //delay(100);

  // Output calibration
  System.out.printf("Gyroscope calibration was written in EEPROM on your MotionerIMU!\n");
  System.out.printf("Gyroscope average offset = {%.6g, %.6g, %.6g};\n", gyroAverage[0], gyroAverage[1], gyroAverage[2]);
  println("\n");
  resynch();
}

