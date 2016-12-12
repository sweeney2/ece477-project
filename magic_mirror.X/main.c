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

//#include "config_bits.h"
#include "apds9960.h"
#include "bluetooth.h"
#include "uart.h"
#include <xc.h>
#include "mcc_generated_files/mcc.h"
#include "mcc_generated_files/i2c2.h"

// Select Internal FRC at POR
//_FOSCSEL(FNOSC_FRC & IESO_OFF);
// Enable Clock Switching and Configure Primary Oscillator in XT mode
//_FOSC(FCKSM_CSECMD & OSCIOFNC_OFF & POSCMD_NONE);

//functions
void handleGesture();
// Pins

// Constants
_FOSC(FCKSM_CSECMD & OSCIOFNC_OFF & POSCMD_NONE);
_FOSCSEL(FNOSC_FRCDIVN & IESO_OFF);
// Global Variables
//SparkFun_APDS9960 apds = SparkFun_APDS9960();
int isr_flag = 0;

void Initialize(void)
{
//    AD1PCFG = 0xffcf;               //All digital I/O except AN4 for temperature sensor and AN5 for potentiometer
//    LATA = 0x0000;                  //All LEDs off
//    TRISA = 0xff00;                 //RA0-RA7 are active high LED outputs
//    LATB = 0x0002;                  //RB1 Wake from Dormant high
//    TRISB = 0xfffd;                 //RB1 is an output
//    LATD = 0x0000;                  //Start with RD15 U1RTS low
//    TRISD = 0x7fff;                 //RD15 is U1RTS output, RD14 is U1CTS input
//    LATF = 0x000b;                  //RF0 CMD, RF3 U1TX, RF1 WAKE_NOTSLEEP pin high
//    TRISF = 0xfff4;                 //RF2 U1RX input, RF6 not connected input, RF7 PIO8 input, RF8 duplicate AWAKE input
    //Configure clock
    //PLLFBD = 50;                      //M = 52;
    //CLKDIVbits.PLLPOST = 4;           //N2 = 6;
    //CLKDIVbits.PLLPRE = 0;            //N1 = 2;
    
    CLKDIVbits.FRCDIV = 0b000;
    //__builtin_write_OSCCONH(0x02);
    OSCCONbits.NOSC = 0b111;
    //__builtin_write_OSCCONL(OSCCON | 0x01);
    OSCCONbits.OSWEN = 1;
    while(OSCCONbits.COSC != 0b111);
    /*switch (OSCCONbits.COSC) {
        case 0b000: while(1);
        case 0b001: while(1);
        case 0b010: while(1);
        case 0b011: while(1);
        case 0b100: while(1);
        case 0b101: while(1);
        case 0b110: while(1);
        case 0b111: while(1);
    }*/
    //while (OSCCONbits.LOCK != 1);
    
    TRISE = 0;
    ANSELE = 0;
    TRISG = 0;
    ANSELG = 0;
    ANSELB = 0;
    TRISF = 0;
    TRISD = 0;
    ANSELD = 0;

    UART_Init();                    //Initialize the UART
    Iimers_Init();                  //Initialize the timers

    INTCON1 = 0;                    //Interrupt nesting enabled
    INTCON2 = 0;                    //GIE off, ext interrupts on rising edge
    INTCON2bits.GIE = 1;            //Global enable interrupts (for dsPIC33EP and PIC24EP devices
}

int main(void) {
  
    char bTVersion[9] = "\0";                               //String of text representing BT module firmware version
    char errorCode = FALSE;// gotPacket = FALSE;              //errorCode TRUE if no error, gotPacket TRUE if valid packet

                                             

    
    
  //Initialize Timers
  Iimers_Init();
  //Initialize system functions
  SYSTEM_Initialize();
  
  Initialize();                                    //Initialize the pins and peripherals
  
  //Initialize the LCD screen
 //LCD_Initialize();
  TRISEbits.TRISE5 = 0;
  //LCD_ClearScreen();
 

  // Initialize APDS-9960 (configure I2C and initial values)
  if ( init() ) {
      LATEbits.LATE5 = 1;
    /*
    LCD_CursorToLine2();
    LCD_MoveCursorRight();
    LCD_MoveCursorRight();
    LCD_MoveCursorRight();
    LCD_WriteString("Gesture Good!");
     */
  } else {
      //LATEbits.LATE5 = 0;
      //T4CON = TIMER_OFF;
      /*
    LCD_CursorToLine2();
    LCD_MoveCursorRight();
    LCD_MoveCursorRight();
    LCD_MoveCursorRight();
    LCD_WriteString("Gesture Bad!");
    */
  }

  // Start running the APDS-9960 gesture sensor engine
  if ( enableGestureSensor(true) ) {
      LATEbits.LATE5 = 1;
    /*
    LCD_CursorToLine2();
    LCD_MoveCursorRight();
    LCD_MoveCursorRight();
    LCD_MoveCursorRight();
    LCD_WriteString("Gesture Run!");
     */
  } else {
      //LATEbits.LATE5 = 0;
    //T4CON = TIMER_OFF;
    /*
    LCD_CursorToLine2();
    LCD_MoveCursorRight();
    LCD_MoveCursorRight();
    LCD_MoveCursorRight();
    LCD_WriteString("Gesture Er");
     */
  }
  
  // wait for BT
  LATEbits.LATE7 = 1;
  LATGbits.LATG7 = 1;
  LATFbits.LATF0 = 1;
  LATDbits.LATD3 = 1;
  Wait1ms(100);
  
  while(IsNewRx1Data())                                    //While buffer contains old data
      ReadRx1Buffer();                                     //Keep reading until empty
  while(IsNewRx2Data())                                    //While buffer contains old data
      ReadRx2Buffer();                                     //Keep reading until empty
    
//    errorCode = BT_SetupModule(bTVersion);                  //Set up RN4020 module
    do {
        errorCode = BT1_SetupModule(bTVersion);
    } while(!(errorCode == TRUE));
    do {
        errorCode = BT2_SetupModule(bTVersion);
    } while(!(errorCode == TRUE));
    UART_EnableFlowControl();                               //Enable flow control since module now set up to use it
    Wait1ms(100);
  
  // MAIN LOOP!!!
  while(1)
  {
     if( isr_flag == 1 ) 
    {
        handleGesture();
        isr_flag = 0;
    }
     if(IsNewRx2Data()) {
         uint8_t app[30], title[30], subtitle[30];
         uint16_t app_len, title_len, subtitle_len;
         
         BT2_GetResponse(app);
         app += 8;
         for (app_len = 0; *(app + app_len) != 0x2E; app_len++);
         
         BT2_GetResponse(title);
         title += 8;
         for (title_len = 0; *(title + title_len) != 0x2E; title_len++);
         
         BT2_GetResponse(subtitle);
         subtitle += 8;
         for (subtitle_len = 0; *(subtitle + subtitle_len) != 0x2E; subtitle_len++);
         
         I2C2_QueueMessage(app, app_len, title, title_len, subtitle, subtitle_len);
     }
  }
  
  return 0;
}

static uint8_t app[20] = {0x61, 0x70, 0x70, 0x21, 0x21, 0x21, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static uint8_t title[20] = {0x74, 0x69, 0x74, 0x6c, 0x65, 0x21, 0x21, 0x21, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static uint8_t subtitle[20] = {0x73, 0x75, 0x62, 0x74, 0x69, 0x74, 0x6c, 0x65, 0x21, 0x21, 0x21, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
void handleGesture() {
    if ( isGestureAvailable() ) {
    switch ( readGesture() ) {
      case DIR_UP:
          I2C2_QueueByte(0x03);
          /*
        LCD_CursorToLine2();
        LCD_WriteString("                ");
        LCD_CursorToLine2();
        LCD_MoveCursorRight();
        LCD_MoveCursorRight();
        LCD_MoveCursorRight();
        LCD_MoveCursorRight();
        LCD_MoveCursorRight();
        LCD_WriteString("UP");
           */
        break;
      case DIR_DOWN:
          I2C2_QueueByte(0x04);
          /*
        LCD_CursorToLine2();
        LCD_WriteString("                ");
        LCD_CursorToLine2();
        LCD_MoveCursorRight();
        LCD_MoveCursorRight();
        LCD_MoveCursorRight();
        LCD_MoveCursorRight();
        LCD_MoveCursorRight();
        LCD_WriteString("DOWN");
           */
        break;
      case DIR_LEFT:
          I2C2_QueueByte(0x01);
          /*
        LCD_CursorToLine2();
        LCD_WriteString("                ");
        LCD_CursorToLine2();
        LCD_MoveCursorRight();
        LCD_MoveCursorRight();
        LCD_MoveCursorRight();
        LCD_MoveCursorRight();
        LCD_MoveCursorRight();
        LCD_WriteString("LEFT");
           */
        break;
      case DIR_RIGHT:
          I2C2_QueueByte(0x02);
          /*
        LCD_CursorToLine2();
        LCD_WriteString("                ");
        LCD_CursorToLine2();
        LCD_MoveCursorRight();
        LCD_MoveCursorRight();
        LCD_MoveCursorRight();
        LCD_MoveCursorRight();
        LCD_MoveCursorRight();
        LCD_WriteString("RIGHT");
           */
        break;
      default:
        //LCD_CursorToLine2();
        //LCD_WriteString("                ");
        //LCD_CursorToLine2();
        //LCD_MoveCursorRight();
        //LCD_MoveCursorRight();
        //LCD_MoveCursorRight();
        //LCD_MoveCursorRight();
        //LCD_MoveCursorRight();
        //LCD_WriteString("NONE");
        break;
    }
  }
}

/**
  Interrupt Handler for EX_INT0 - INT0
*/
void __attribute__ ( ( interrupt, no_auto_psv ) ) _INT0Interrupt(void)
{
    //***User Area Begin->code: INT0 - External Interrupt 0***
    isr_flag = 1;
    //***User Area End->code: INT0 - External Interrupt 0***
    EX_INT0_InterruptFlagClear();
}

