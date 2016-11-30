/****************************************************************
GestureTest.ino
APDS-9960 RGB and Gesture Sensor
Shawn Hymel @ SparkFun Electronics
May 30, 2014
https://github.com/sparkfun/APDS-9960_RGB_and_Gesture_Sensor

Tests the gesture sensing abilities of the APDS-9960. Configures
APDS-9960 over I2C and waits for gesture events. Calculates the
direction of the swipe (up, down, left, right) and displays it
on a serial console. 

To perform a NEAR gesture, hold your hand
far above the sensor and move it close to the sensor (within 2
inches). Hold your hand there for at least 1 second and move it
away.

To perform a FAR gesture, hold your hand within 2 inches of the
sensor for at least 1 second and then move it above (out of
range) of the sensor.

Hardware Connections:

IMPORTANT: The APDS-9960 can only accept 3.3V!

 Arduino Pin  APDS-9960 Board  Function

 3.3V         VCC              Power
 GND          GND              Ground
 A4           SDA              I2C Data
 A5           SCL              I2C Clock
 2            INT              Interrupt

Resources:
Include Wire.h and SparkFun_APDS-9960.h

Development environment specifics:
Written in Arduino 1.0.5
Tested with SparkFun Arduino Pro Mini 3.3V

This code is beerware; if you see me (or any other SparkFun 
employee) at the local, and you've found our code helpful, please
buy us a round!

Distributed as-is; no warranty is given.
****************************************************************/

#include "apds9960.h"

//functions
void handleGesture();
// Pins
#define APDS9960_INT    2 // Needs to be an interrupt pin

// Constants

// Global Variables
//SparkFun_APDS9960 apds = SparkFun_APDS9960();
int isr_flag = 0;

int main(void) {

  //Initialize the LCD screen
  LCD_Initialize();
  // Initialize Serial port
  

  // Initialize interrupt service routine
  //attachInterrupt(0, interruptRoutine, FALLING);

  // Initialize APDS-9960 (configure I2C and initial values)
  if ( init() ) {
    LCD_WriteString("APDS-9960 initialization complete");
  } else {
    LCD_WriteString("Something went wrong during APDS-9960 init!");
  }

  // Start running the APDS-9960 gesture sensor engine
  if ( enableGestureSensor(true) ) {
    LCD_WriteString("Gesture sensor is now running");
  } else {
    LCD_WriteString("Something went wrong during gesture sensor init!");
  }
  
  return 0;
}

void handleGesture() {
    if ( isGestureAvailable() ) {
    switch ( readGesture() ) {
      case DIR_UP:
        LCD_WriteString("UP");
        break;
      case DIR_DOWN:
        LCD_WriteString("DOWN");
        break;
      case DIR_LEFT:
        LCD_WriteString("LEFT");
        break;
      case DIR_RIGHT:
        LCD_WriteString("RIGHT");
        break;
      default:
        LCD_WriteString("NONE");
    }
  }
}

void loop() {
  if( isr_flag == 1 ) {
    //detachInterrupt(0);
    handleGesture();
    isr_flag = 0;
    //attachInterrupt(0, interruptRoutine, FALLING);
  }
}

void interruptRoutine() {
  isr_flag = 1;
}
