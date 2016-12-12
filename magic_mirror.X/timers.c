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
 * File:        timers.c
 * Date:        July 24, 2014
 * Compiler:    XC16 v1.21
 *
 * Timer functions
 *
 */

#include <xc.h>
#include "definitions.h"
#include "timers.h"

static volatile unsigned int TickCount[3] = {0, 0, 0};      //We will keep track of three software timers

//**********************************************************************************************************************
// Initialize the timers

void Iimers_Init(void)
{
    //Timer 5 is used for interrupt based timers counting 1ms intervals to a resolution of 0.1ms
    T5CON = TIMER_OFF;                      //Timer 4 off
    TMR5 = 0;                               //Clear timer 4
    PR5 = TIMER_100US_PERIOD;               //Set the period value for 100us
    T5CON = TIMER_ON_PRESCALE1;             //using 1:1 prescaler and turn on timer 5
    IFS1bits.T5IF = 0;                      //Clear the interrpt flag
    IEC1bits.T5IE = 1;                      //Enable the timer 5 interrupt (global enable is in main initialization)
}

//**********************************************************************************************************************
// Start one of the software timers

void StartTimer(unsigned char Timer, unsigned int Count)
{
    TickCount[Timer] = Count * 10; //Interrupt is every 100us but StartTimer() takes multiple of 1ms so multiply by 10;
    IFS1bits.T5IF = 0;                      //Clear the interrpt flag
    IEC1bits.T5IE = 1;                      //Enable the timer 5 interrupt (global enable is in main initialization)
}

//**********************************************************************************************************************
// Check if one of the software software timers has timed out

unsigned char TimerDone(unsigned char Timer)
{
    if(TickCount[Timer] == 0)               //Check if counted down to zero
        return TRUE;                        //then return true
    return FALSE;                           //else return false
}

//**********************************************************************************************************************
// Simple delay for n milliseconds

void Wait1ms(unsigned int NumMilliseconds)
{
    StartTimer(2, NumMilliseconds);         //Start software timer 2
    while(!TimerDone(2));                   //and wait for it to count down
}

//**********************************************************************************************************************
// Simple delay for n microseconds

void Wait1us(unsigned int NumMicroseconds)
{
    if(NumMicroseconds <= 0)                //Check that the value is valid
        return;                             //else return immediately
//    else if (NumMicroseconds > 10000)     //If over 10ms then use millisecond delay instead
//        Wait1ms(NumMicroseconds / 1000);
    else
    {
        T1CON = TIMER_OFF;                  //Use hardware timer 1
        TMR1 = 0;                           //Clear the timer
        PR1 = NumMicroseconds << 1;         //Double to get the count required because we are clocking at 2MHz
        IFS0bits.T1IF = 0;                  //Clear the interrupt flag
        T1CON = TIMER_ON_PRESCALE8;         //Timer clocks at 2 MHz with 16MHz Fcy
        while(!IFS0bits.T1IF);              //Wait for the interrupt flag to indicate the period has been reached
        T1CON = TIMER_OFF;                  //Stop the timer
        IFS0bits.T1IF = 0;                  //Clear the interrupt flag
    }
}

//**********************************************************************************************************************
// Timer 5 interrupt routine

void _ISR_NO_AUTO_PSV _T5Interrupt(void)
{
    IFS1bits.T5IF = 0;                      //Clear the interrupt flag
    if(TickCount[0] != 0)                   //If software timer 0 has not reached zero then
        TickCount[0]--;                     //decrement it
    if(TickCount[1] != 0)                   //If software timer 1 has not reached zero then
        TickCount[1]--;                     //decrement it
    if(TickCount[2] != 0)                   //If software timer 2 has not reached zero then
        TickCount[2]--;                     //decrement it
}

