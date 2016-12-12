/**
  @Generated MPLAB(c) Code Configurator Source File

  @Company:
    Microchip Technology Inc.

  @File Name:
    mcc.c

  @Summary:
    This is the mcc.c file generated using MPLAB(c) Code Configurator

  @Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  MPLAB(c) Code Configurator - 4.0
        Device            :  PIC24FJ128GA010
        Driver Version    :  1.02
    The generated drivers are tested against the following:
        Compiler          :  XC16 1.26
        MPLAB             :  MPLAB X 3.40
*/

/*
    (c) 2016 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
*/

// Configuration bits: selected in the GUI

// CONFIG2
//#pragma config POSCMOD = NONE    // Primary Oscillator Select->Primary oscillator disabled
//#pragma config OSCIOFNC = OFF    // Primary Oscillator Output Function->OSC2/CLKO/RC15 functions as CLKO (FOSC/2)
//#pragma config FCKSM = CSDCMD    // Clock Switching and Monitor->Clock switching and Fail-Safe Clock Monitor are disabled
//#pragma config FNOSC = 0b111    // Oscillator Select->Fast RC Oscillator with Postscaler (FRCDIV)
//pragma config IESO = ON    // Internal External Switch Over Mode->IESO mode (Two-Speed Start-up) enabled

// CONFIG1
//#pragma config WDTPOST = 15    // Watchdog Timer Postscaler->1:32768
//#pragma config WDTPRE = 1    // WDT Prescaler->Prescaler ratio of 1:128
#pragma config WINDIS = ON    // Watchdog Timer Window->Standard Watchdog Timer enabled,(Windowed-mode is disabled)
#pragma config FWDTEN = OFF    // Watchdog Timer Enable->Watchdog Timer is disabled
//#pragma config ICS = PGx2    // Comm Channel Select->Emulator/debugger uses EMUC2/EMUD2
//#pragma config COE = OFF    // Set Clip On Emulation Mode->Reset Into Operational Mode
//#pragma config BKBUG = OFF    // Background Debug->Device resets into Operational mode
#pragma config GWRP = OFF    // General Code Segment Write Protect->Writes to program memory are allowed
//#pragma config GCP = OFF    // General Code Segment Code Protect->Code protection is disabled
#pragma config JTAGEN = OFF    // JTAG Port Enable->JTAG port is disabled



#include "mcc.h"

void SYSTEM_Initialize(void)
{
    PIN_MANAGER_Initialize();
    OSCILLATOR_Initialize();
    INTERRUPT_Initialize();
    I2C1_Initialize();
    I2C2_Initialize();
    EXT_INT_Initialize();
}

void OSCILLATOR_Initialize(void)
{
    // NOSC FRCDIV; SOSCEN disabled; OSWEN Switch is Complete; 
    //__builtin_write_OSCCONL((uint8_t) (0x0700 & 0x00FF));
    
    //PLLFBD=50;            // M=52
    //CLKDIVbits.PLLPOST=2; // N2=6
    //CLKDIVbits.PLLPRE=0;  // N1=2
    // RCDIV FRC/2; DOZE 1:8; DOZEN disabled; ROI disabled; 
    CLKDIVbits.FRCDIV = 0b000;
    // TUN Center frequency; 
    OSCCONbits.NOSC = 0b111;
    OSCCONbits.OSWEN = 1;
    while(OSCCONbits.COSC != 0b111);
    // WDTO disabled; TRAPR disabled; SWDTEN disabled; EXTR disabled; POR disabled; SLEEP disabled; BOR disabled; IDLE disabled; IOPUWR disabled; VREGS disabled; CM disabled; SWR disabled; 
    //RCON = 0x0000;
    
    // Initiate Clock Switch to FRC oscillator with PLL (NOSC=0b001)
    //__builtin_write_OSCCONH(0x01);
    //__builtin_write_OSCCONL(OSCCON | 0x01);
    // Wait for Clock switch to occur
    //while (OSCCONbits.COSC!= 0b001);
    // Wait for PLL to lock
    //while (OSCCONbits.LOCK!= 1);
}

/**
 End of File
*/