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
 * File:        UART.c
 * Date:        July 24, 2014
 * Compiler:    XC16 v1.21
 *
 * UART functions
 *
 */

#include <xc.h>
#include "definitions.h"

static volatile unsigned char rx1Buffer[SIZE_RxBuffer], *rx1BufRdPtr, *rx1BufWrPtr;    //Buffer and pointers for received bytes
static volatile unsigned char tx1Buffer[SIZE_TxBuffer], *tx1BufRdPtr, *tx1BufWrPtr;    //Buffer and pointers for bytes to transmit

static volatile unsigned char rx2Buffer[SIZE_RxBuffer], *rx2BufRdPtr, *rx2BufWrPtr;    //Buffer and pointers for received bytes
static volatile unsigned char tx2Buffer[SIZE_TxBuffer], *tx2BufRdPtr, *tx2BufWrPtr;    //Buffer and pointers for bytes to transmit

//**********************************************************************************************************************
// Initialize the UART to communitcate with the Bluetooth module

void UART_Init(void)
{
    rx1BufRdPtr = rx1BufWrPtr = &rx1Buffer[0]; //Initialize the pointers
    tx1BufRdPtr = tx1BufWrPtr = &tx1Buffer[0];
    rx2BufRdPtr = rx2BufWrPtr = &rx2Buffer[0]; //Initialize the pointers
    tx2BufRdPtr = tx2BufWrPtr = &tx2Buffer[0];
    
    U1BRG = BRG_115200;                     //Baud rate 115,200 - actually 115,384 baud 0.16% error
    U2BRG = BRG_115200;
//    U1MODE = 0x8208;                        //Enable UART, use RTC/CTS flow control, BRGH = 1 for high speed baud mode
    U1MODE = 0x8008;                        //Enable UART, no flow control, BRGH = 1 for high speed baud mode
    U2MODE = 0x8008;
    U1STA = 0x0400;                         //Enable transmit
    U2STA = 0x0400;
    RPINR18 = 0x002F;
    RPOR8 = 0x0100;
    RPINR19 = 0x0053;
    RPOR5 = 0x0003;
    IFS0bits.U1RXIF = 0;                    //Clear UART 1 Receive interrupt flag
    IEC0bits.U1RXIE = 1;                    //Enable UART 1 Receive interrupt
    IFS1bits.U2RXIF = 0;
    IEC1bits.U2RXIE = 1;
}

//**********************************************************************************************************************
// Start transmission by enabling the UART transmit interrupt

void UART_EnableFlowControl(void)
{
    U1MODE = (U1MODE & 0xf7ff) | 0x0200;    //Enable RTC/CTS flow control
    U2MODE = (U2MODE & 0xf7ff) | 0x0200;
}

//**********************************************************************************************************************
// Start transmission by enabling the UART transmit interrupt

void UART_Tx1Start(void)
{
    IEC0bits.U1TXIE = 1;                    //Enable transmit interrupts
}

void UART_Tx2Start(void)
{
    IEC1bits.U2TXIE = 1;                    //Enable transmit interrupts
}

//**********************************************************************************************************************
// See if there is one or more bytes in the receive buffer

int IsNewRx1Data(void)
{
    asm volatile ("disi #0x0002");          //Disable interrupts while comparing the pointers
    if(rx1BufRdPtr == rx1BufWrPtr)            //Check if pointers are the same
        return(FALSE);                      //No bytes in the buffer so return false
    return(TRUE);                           //If pointers are different then there are bytes in the buffer
}

int IsNewRx2Data(void)
{
    asm volatile ("disi #0x0002");          //Disable interrupts while comparing the pointers
    if(rx2BufRdPtr == rx2BufWrPtr)            //Check if pointers are the same
        return(FALSE);                      //No bytes in the buffer so return false
    return(TRUE);                           //If pointers are different then there are bytes in the buffer
}

//**********************************************************************************************************************
// Read a byte from the receive buffer

int ReadRx1Buffer(void)
{
    int Temp;

    asm volatile ("disi #0x0002");          //Disable interrupts while comparing the pointers
    if(rx1BufRdPtr == rx1BufWrPtr)            //For safety, do not allow read of empty buffer
        return(0);                          //Return zero if there is nothing in the buffer
    Temp = *rx1BufRdPtr++;                   //Get the byte and increment the pointer
    if (rx1BufRdPtr > &rx1Buffer[SIZE_RxBuffer - 1]) //Check if at end of buffer
        rx1BufRdPtr = &rx1Buffer[0];          //then wrap the pointer to beginning
    return(Temp);
}

int ReadRx2Buffer(void)
{
    int Temp;

    asm volatile ("disi #0x0002");          //Disable interrupts while comparing the pointers
    if(rx2BufRdPtr == rx2BufWrPtr)            //For safety, do not allow read of empty buffer
        return(0);                          //Return zero if there is nothing in the buffer
    Temp = *rx2BufRdPtr++;                   //Get the byte and increment the pointer
    if (rx2BufRdPtr > &rx2Buffer[SIZE_RxBuffer - 1]) //Check if at end of buffer
        rx2BufRdPtr = &rx2Buffer[0];          //then wrap the pointer to beginning
    return(Temp);
}

//**********************************************************************************************************************
// Write a byte to the transmit buffer

void WriteTx1Buffer(unsigned char TxByte)
{
    *tx1BufWrPtr++ = TxByte;                 //Put the byte in the transmit buffer and increment the pointer
    if (tx1BufWrPtr > &tx1Buffer[SIZE_RxBuffer - 1]) //Check if at end of buffer
        tx1BufWrPtr = &tx1Buffer[0];          //Wrap pointer to beginning
}

void WriteTx2Buffer(unsigned char TxByte)
{
    *tx2BufWrPtr++ = TxByte;                 //Put the byte in the transmit buffer and increment the pointer
    if (tx2BufWrPtr > &tx2Buffer[SIZE_RxBuffer - 1]) //Check if at end of buffer
        tx2BufWrPtr = &tx2Buffer[0];          //Wrap pointer to beginning
}

//**********************************************************************************************************************
// Interrupt routine for UART receive interrupts

void _ISR_NO_AUTO_PSV _U1RXInterrupt(void)
{
    IFS0bits.U1RXIF = 0;                    //Clear UART 1 Receive interrupt flag
    *rx1BufWrPtr++ = U1RXREG;                //Put received character in the buffer
    if (rx1BufWrPtr > &rx1Buffer[SIZE_RxBuffer - 1])  //Check if end of buffer
        rx1BufWrPtr = &rx1Buffer[0];          //Wrap pointer to beginning
}

void _ISR_NO_AUTO_PSV _U2RXInterrupt(void)
{
    IFS1bits.U2RXIF = 0;                    //Clear UART 1 Receive interrupt flag
    *rx2BufWrPtr++ = U2RXREG;                //Put received character in the buffer
    if (rx2BufWrPtr > &rx2Buffer[SIZE_RxBuffer - 1])  //Check if end of buffer
        rx2BufWrPtr = &rx2Buffer[0];          //Wrap pointer to beginning
}

//**********************************************************************************************************************
// Interrupt routine for UART transmit interrupts

void _ISR_NO_AUTO_PSV _U1TXInterrupt(void)
{
    if (tx1BufRdPtr != tx1BufWrPtr)           //Check if more data is in the buffer
    {
        IFS0bits.U1TXIF = 0;                //Clear UART 1 Receive interrupt flag
        U1TXREG = *tx1BufRdPtr++;            //Get next byte from the buffer
        if (tx1BufRdPtr > &tx1Buffer[SIZE_TxBuffer - 1])  //Check if end of buffer
            tx1BufRdPtr = &tx1Buffer[0];      //Wrap pointer to beginning
    }
    else
        IEC0bits.U1TXIE = 0;                //No data so stop interrupts
}

void _ISR_NO_AUTO_PSV _U2TXInterrupt(void)
{
    if (tx2BufRdPtr != tx2BufWrPtr)           //Check if more data is in the buffer
    {
        IFS1bits.U2TXIF = 0;                //Clear UART 1 Receive interrupt flag
        U2TXREG = *tx2BufRdPtr++;            //Get next byte from the buffer
        if (tx2BufRdPtr > &tx2Buffer[SIZE_TxBuffer - 1])  //Check if end of buffer
            tx2BufRdPtr = &tx2Buffer[0];      //Wrap pointer to beginning
    }
    else
        IEC1bits.U2TXIE = 0;                //No data so stop interrupts
}

