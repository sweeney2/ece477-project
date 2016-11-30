/*
 * Copyright (C) 2014 Microchip Technology Inc. and its subsidiaries.  You may use this software and any derivatives
 * exclusively with Microchip products.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS
 * SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR
 * PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE,
 * COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF
 * THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON
 * ALL CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY, THAT YOU HAVE PAID
 * DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE TERMS.
 *
 *
 * File:        LCD.c
 * Date:        July 24, 2014
 * Compiler:    XC16 v1.21
 *
 * LCD functions for Explorer-16 board
 *
 */

#include <xc.h>
#include "lcd.h"
#include "timers.h"

// Control signal data pins 
#define  LCD_RW         LATDbits.LATD5       // LCD R/W signal
#define  LCD_RS         LATBbits.LATB15      // LCD RS signal
#define  LCD_EN         LATDbits.LATD4       // LCD E signal

// Control signal pin direction 
#define  LCD_RW_TRIS    TRISDbits.TRISD5
#define  LCD_RS_TRIS    TRISBbits.TRISB15
#define  LCD_EN_TRIS    TRISDbits.TRISD4

// Data signals and pin direction
#define  LCD_DATA_BUS   LATE                // Port for LCD data
//#define  LCD_DATA_TRIS  TRISE               // I/O setup for data Port

//**********************************************************************************************************************
// Initialize the LCD

void LCD_Initialize(void)
{
    // 15mS delay after Vdd reaches nnVdc before proceeding with LCD initialization
    // not always required and is based on system Vdd rise rate
    Wait1ms(15);                // 15ms delay

    /* set initial states for the data and control pins */
    LATE &= 0xFF00;
    LCD_RW = 0;                 // R/W state set low
    LCD_RS = 0;                 // RS state set low
    LCD_EN = 0;                 // E state set low

    /* set data and control pins to outputs */
    TRISE &= 0xFF00;
    LCD_RW_TRIS = 0;            // RW pin set as output
    LCD_RS_TRIS = 0;            // RS pin set as output
    LCD_EN_TRIS = 0;            // E pin set as output

    /* 1st LCD initialization sequence */
    LCD_DATA_BUS &= 0xFF00;
    LCD_DATA_BUS |= 0x0038;     // Put 0x38 on the data bus
    LCD_EN = 1;                 // Enable pin high
    Nop();                      // Short delay for enable pulse
    Nop();
    Nop();
    LCD_EN = 0;                 // Enable pin low
    Wait1ms(5);                 // 5ms delay

    /* 2nd LCD initialization sequence */
    LCD_DATA_BUS &= 0xFF00;
    LCD_DATA_BUS |= 0x0038;     // Put 0x38 on the data bus
    LCD_EN = 1;                 // Enable pin high
    Nop();                      // Short delay for enable pulse
    Nop();
    Nop();
    LCD_EN = 0;                 // Enable pin low
    Wait1us(200);               // 200uS delay

    /* 3rd LCD initialization sequence */
    LCD_DATA_BUS &= 0xFF00;
    LCD_DATA_BUS |= 0x0038;     // Put 0x38 on the data bus
    LCD_EN = 1;                 // Enable pin high
    Nop();                      // Short delay for enable pulse
    Nop();
    Nop();
    LCD_EN = 0;                 // Enable pin low
    Wait1us(200); // 200uS delay

    LCD_WriteCommand(0x38);     // Set function
    LCD_WriteCommand(0x0C);     // Display on/off control, cursor blink off (0x0C)
    LCD_WriteCommand(0x06);     // Set entry mode (0x06)
}

//**********************************************************************************************************************
// Write a single command byte to the LCD

void LCD_WriteCommand(char cmd)
{
    //	TRISD &= 0xFF00;        // ensure RD0 - RD7 are outputs
    LCD_DATA_BUS &= 0xFF00;
    LCD_DATA_BUS |= cmd;        // Put command byte on the data bus
    LCD_RW = 0;                 // RW is 0 for write
    LCD_RS = 0;                 // RS is 0 for control register
    LCD_EN = 1;                 // Enable pin high
    Nop();                      // Short delay for enable pulse
    Nop();
    Nop();
    LCD_EN = 0;                 // Enable pin low
    Wait1ms(5); // 5ms delay
}

//**********************************************************************************************************************
// Write a single data byte to the LCD

void LCD_WriteData(char data)
{
    //	TRISD &= 0xFF00;        // ensure RD0 - RD7 are outputs
    LCD_RW = 0;                 // RW is 0 for write
    LCD_RS = 1;                 // RS is 1 for data
    LCD_DATA_BUS &= 0xFF00;
    LCD_DATA_BUS |= data;       // Put data byte on the data bus
    LCD_EN = 1;                 // Enable pin high
    Nop();                      // Short delay for enable pulse
    Nop();
    Nop();
    LCD_EN = 0;                 // Enable pin low
    LCD_RS = 0;                 // negate register select to 0
    Wait1us(400);               // 400uS delay
}

//**********************************************************************************************************************
// Write a string of text to the LCD

void LCD_WriteString(char *data) {
    int i;
    for (i = 0; i < 16; i++) {      //Write up to 16 bytes
        if (*data != '\0')          //Check for the end of the text string
            LCD_WriteData(*data++); //Write the byte if it is not null
        else                        //else
            break;                  //we are done if the byte is null
    }
}

