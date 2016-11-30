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

static volatile unsigned char rxBuffer[SIZE_RxBuffer], *rxBufRdPtr, *rxBufWrPtr;    //Buffer and pointers for received bytes
static volatile unsigned char txBuffer[SIZE_TxBuffer], *txBufRdPtr, *txBufWrPtr;    //Buffer and pointers for bytes to transmit

//**********************************************************************************************************************
// Initialize the UART to communitcate with the Bluetooth module

void UART_Init(void)
{
    rxBufRdPtr = rxBufWrPtr = &rxBuffer[0]; //Initialize the pointers
    txBufRdPtr = txBufWrPtr = &txBuffer[0];
    U1BRG = BRG_115200;                     //Baud rate 115,200 - actually 115,384 baud 0.16% error
//    U1MODE = 0x8208;                        //Enable UART, use RTC/CTS flow control, BRGH = 1 for high speed baud mode
    U1MODE = 0x8008;                        //Enable UART, no flow control, BRGH = 1 for high speed baud mode
    U1STA = 0x0400;                         //Enable transmit
    IFS0bits.U1RXIF = 0;                    //Clear UART 1 Receive interrupt flag
    IEC0bits.U1RXIE = 1;                    //Enable UART 1 Receive interrupt
}

//**********************************************************************************************************************
// Start transmission by enabling the UART transmit interrupt

void UART_EnableFlowControl(void)
{
    U1MODE = (U1MODE & 0xf7ff) | 0x0200;    //Enable RTC/CTS flow control
}

//**********************************************************************************************************************
// Start transmission by enabling the UART transmit interrupt

void UART_TxStart(void)
{
    IEC0bits.U1TXIE = 1;                    //Enable transmit interrupts
}

//**********************************************************************************************************************
// See if there is one or more bytes in the receive buffer

int IsNewRxData(void)
{
    asm volatile ("disi #0x0002");          //Disable interrupts while comparing the pointers
    if(rxBufRdPtr == rxBufWrPtr)            //Check if pointers are the same
        return(FALSE);                      //No bytes in the buffer so return false
    return(TRUE);                           //If pointers are different then there are bytes in the buffer
}

//**********************************************************************************************************************
// Read a byte from the receive buffer

int ReadRxBuffer(void)
{
    int Temp;

    asm volatile ("disi #0x0002");          //Disable interrupts while comparing the pointers
    if(rxBufRdPtr == rxBufWrPtr)            //For safety, do not allow read of empty buffer
        return(0);                          //Return zero if there is nothing in the buffer
    Temp = *rxBufRdPtr++;                   //Get the byte and increment the pointer
    if (rxBufRdPtr > &rxBuffer[SIZE_RxBuffer - 1]) //Check if at end of buffer
        rxBufRdPtr = &rxBuffer[0];          //then wrap the pointer to beginning
    return(Temp);
}

//**********************************************************************************************************************
// Write a byte to the transmit buffer

void WriteTxBuffer(unsigned char TxByte)
{
    *txBufWrPtr++ = TxByte;                 //Put the byte in the transmit buffer and increment the pointer
    if (txBufWrPtr > &txBuffer[SIZE_RxBuffer - 1]) //Check if at end of buffer
        txBufWrPtr = &txBuffer[0];          //Wrap pointer to beginning
}

//**********************************************************************************************************************
// Interrupt routine for UART receive interrupts

void _ISR_NO_AUTO_PSV _U1RXInterrupt(void)
{
    IFS0bits.U1RXIF = 0;                    //Clear UART 1 Receive interrupt flag
    *rxBufWrPtr++ = U1RXREG;                //Put received character in the buffer
    if (rxBufWrPtr > &rxBuffer[SIZE_RxBuffer - 1])  //Check if end of buffer
        rxBufWrPtr = &rxBuffer[0];          //Wrap pointer to beginning
}

//**********************************************************************************************************************
// Interrupt routine for UART transmit interrupts

void _ISR_NO_AUTO_PSV _U1TXInterrupt(void)
{
    if (txBufRdPtr != txBufWrPtr)           //Check if more data is in the buffer
    {
        IFS0bits.U1TXIF = 0;                //Clear UART 1 Receive interrupt flag
        U1TXREG = *txBufRdPtr++;            //Get next byte from the buffer
        if (txBufRdPtr > &txBuffer[SIZE_TxBuffer - 1])  //Check if end of buffer
            txBufRdPtr = &txBuffer[0];      //Wrap pointer to beginning
    }
    else
        IEC0bits.U1TXIE = 0;                //No data so stop interrupts
}

