/**
  I2C2 Generated Driver File

  @Company
    Microchip Technology Inc.

  @File Name
    i2c2.c

  @Summary
    This is the generated header file for the I2C2 driver using MPLAB(c) Code Configurator

  @Description
    This header file provides APIs for driver for I2C2.
    Generation Information :
        Product Revision  :  MPLAB(c) Code Configurator - 4.0
        Device            :  PIC24FJ128GA010
        Driver Version    :  1.0
    The generated drivers are tested against the following:
        Compiler          :  XC16 1.26
        MPLAB 	          :  MPLAB X 3.40
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

#include "i2c2.h"
#include "../bluetooth.h"

/**
 Section: Data Types
*/

/**
  I2C Slave Driver State Enumeration

  @Summary
    Defines the different states of the i2c slave.

  @Description
    This defines the different states that the i2c slave
    used to process transactions on the i2c bus.
*/
typedef enum
{
    S_SLAVE_IDLE,
    S_SLAVE_RECEIVE_MODE,
    S_SLAVE_TRANSMIT_MODE,
    S_SLAVE_LOW_BYTE_ADDRESS_DETECT,

} I2C_SLAVE_STATES;

/**
 Section: Macro Definitions
*/
/* defined for I2C2 */
#define I2C2_TRANSMIT_REG                       I2C2TRN                 // Defines the transmit register used to send data.
#define I2C2_RECEIVE_REG                        I2C2RCV                 // Defines the receive register used to receive data.

#define I2C2_MASK_REG                           I2C2MSK                 // Defines the address mask register.
#define I2C2_ADDRESS_REG                        I2C2ADD                 // Defines the address register. 

// The following control bits are used in the I2C state machine to manage
// the I2C module and determine next states.
#define I2C2_GENERAL_CALL_ENABLE_BIT            I2C2CONbits.GCEN        // I2C General Call enable control bit.
#define I2C2_10_BIT_ADDRESS_ENABLE_BIT          I2C2CONbits.A10M        // I2C Address Mode (7 or 10 bit address) control bit.
#define I2C2_RELEASE_SCL_CLOCK_CONTROL_BIT      I2C2CONbits.SCLREL      // I2C clock stretch/release control bit.

// The following status bits are used in the I2C state machine to determine
// the next states.

#define I2C2_READ_NOT_WRITE_STATUS_BIT          I2C2STATbits.R_W    // I2C current transaction read/write status bit.
#define I2C2_DATA_NOT_ADDRESS_STATUS_BIT        I2C2STATbits.D_A    // I2C last byte receive was data/address status bit.
#define I2C2_RECEIVE_OVERFLOW_STATUS_BIT        I2C2STATbits.I2COV      // I2C receive buffer overflow status bit.
#define I2C2_GENERAL_CALL_ADDRESS_STATUS_BIT    I2C2STATbits.GCSTAT     // I2C General Call status bit.
#define I2C2_ACKNOWLEDGE_STATUS_BIT             I2C2STATbits.ACKSTAT    // I2C ACK status bit.

/**
 Section: Local Functions
*/

inline void __attribute__ ((always_inline)) I2C2_TransmitProcess(void);
inline void __attribute__ ((always_inline)) I2C2_ReceiveProcess(void);

/**
 Section: Local Variables
*/

static I2C_SLAVE_STATES   i2c2_slave_state;
static uint8_t            *p_i2c2_write_pointer;
static uint8_t            *p_i2c2_read_pointer;

/**
  Prototype:        void I2C2_Initialize(void)
  Input:            none
  Output:           none
  Description:      I2C2_Initialize is an
                    initialization routine that takes inputs from the GUI.
  Comment:          
  Usage:            I2C2_Initialize();
*/
void I2C2_Initialize(void)
{

    // initialize the hardware
    // ACKEN disabled; STREN disabled; GCEN disabled; SMEN disabled; DISSLW enabled; I2CSIDL disabled; ACKDT Sends ACK; SCLREL Holds; RSEN disabled; IPMIEN disabled; A10M 7 Bit; PEN disabled; RCEN disabled; SEN disabled; I2CEN enabled; 
    I2C2CON = 0x8000;
    // P disabled; S disabled; I2COV disabled; IWCOL disabled; 
    I2C2STAT = 0x0000;
    // I2CADD 12; 
    I2C2_SlaveAddressSet(0x000C);
    // AMSK 0; 
    I2C2_SlaveAddressMaskSet(0x0000);

    // make sure this is set first
    i2c2_slave_state = S_SLAVE_IDLE;
    
    I2C2_ReadPointerSet(NULL);
    I2C2_WritePointerSet(NULL);
    
    /* SI2C2 - I2C2 Slave Events */
    // clear the master interrupt flag
    IFS3bits.SI2C2IF = 0;
    // enable the master interrupt
    IEC3bits.SI2C2IE = 1;
    
}


void __attribute__ ( ( interrupt, no_auto_psv ) ) _SI2C2Interrupt ( void )
{

    static bool  prior_address_match = false;
    static bool  not_busy = true;
    uint8_t      dummy;

    // NOTE: The slave driver will always acknowledge 
    //       any address match.

    switch (i2c2_slave_state)
    {
        case S_SLAVE_IDLE:
        case S_SLAVE_RECEIVE_MODE:

            /* When at S_SLAVE_RECEIVE_MODE this mode there
               will be two types of incoming transactions:
               1. Data sent by master
               2. A restart or start detection

               But from the point of view of the firmware, there is
               no difference between S_SLAVE_IDLE and S_SLAVE_RECEIVE_MODE
               states, since the types of incoming transactions will be
               the same so we share the code here.
             */

            if  (
                    // case of 7-bit address detected
                    (   (I2C2_10_BIT_ADDRESS_ENABLE_BIT       == 0) &&
                        (I2C2_DATA_NOT_ADDRESS_STATUS_BIT     == 0)
                    )
                    ||
                    // case of general address call detected
                    (   (I2C2_GENERAL_CALL_ENABLE_BIT         == 1) &&
                        (I2C2_GENERAL_CALL_ADDRESS_STATUS_BIT == 1)
                    )
                )
            {
                            
                if (I2C2_READ_NOT_WRITE_STATUS_BIT == 0)
                {
                    // it is a write, go to receive mode 

                    not_busy = I2C2_StatusCallback(I2C2_SLAVE_RECEIVE_REQUEST_DETECTED);
                    i2c2_slave_state = S_SLAVE_RECEIVE_MODE;

                    if (not_busy == true)
                    {
                        // read the receive register only when
                        // we are ready for the next transaction.
                        // this one is a dummy read
                        dummy = I2C2_RECEIVE_REG;
                    }
                }
                else
                {
                    // it is a read, go to transmit mode 

                    I2C2_StatusCallback(I2C2_SLAVE_TRANSMIT_REQUEST_DETECTED);

                    // during this portion, the master is expecting the
                    // slave for a reply. So the returned status of
                    // the callback at this point cannot be used to 
                    // delay the reply if needed.
                    // In other words, the slave has to reply to the master.
                    // Therefore, the transmit will be performed.
                    
                    I2C2_TransmitProcess();
                    i2c2_slave_state = S_SLAVE_TRANSMIT_MODE;
                }

            }

            else if
               (
                    // case of 10-bit high address detected
                    (   (I2C2_10_BIT_ADDRESS_ENABLE_BIT   == 1)  &&
                        (I2C2_DATA_NOT_ADDRESS_STATUS_BIT == 0)
                    )
               )
            {
                if (I2C2_READ_NOT_WRITE_STATUS_BIT == 0)
                {
                    // it is the detection of high byte address of 
                    // 10-bit address, go to detection of low byte address
                    prior_address_match = false;
                    i2c2_slave_state = S_SLAVE_LOW_BYTE_ADDRESS_DETECT;

                }
                else // if (I2C2_READ_NOT_WRITE_STATUS_BIT == 1)
                {
                    if (prior_address_match == true)
                    {
                        // it is the detection of high byte
                        // address of 10-bit address, but the next
                        // transaction is read transaction (so it
                        // is a restart).
                        // set the transmit register with the data
                        // to transmit then go to transmit mode

                        I2C2_StatusCallback(I2C2_SLAVE_TRANSMIT_REQUEST_DETECTED);

                        // during this portion, the master is expecting the
                        // slave for a reply. So the returned status of
                        // the callback at this point cannot be used to
                        // delay the reply if needed.
                        // In other words, the slave has to reply to the master.
                        // Therefore, the transmit will be performed.

                        I2C2_TransmitProcess();
                        i2c2_slave_state = S_SLAVE_TRANSMIT_MODE;
                    }
                    else
                    {
                        // it is the detection of high byte address of
                        // 10-bit address, but next transaction is a write.
                        // go to detection of low byte address
                        prior_address_match = false;
                        i2c2_slave_state = S_SLAVE_LOW_BYTE_ADDRESS_DETECT;

                    }
                }

                // dummy read is needed
                dummy = I2C2_RECEIVE_REG;
            }

            // this if statement is to make sure we only save incoming
            // data when we are truly in receiving mode
            if (i2c2_slave_state == S_SLAVE_RECEIVE_MODE)
            {
                // case of data received
                if (I2C2_DATA_NOT_ADDRESS_STATUS_BIT == 1)
                {
                    // check if we are overflowing the receive buffer
                    if (I2C2_RECEIVE_OVERFLOW_STATUS_BIT != 1)
                    {
                        I2C2_ReceiveProcess();
                        not_busy = I2C2_StatusCallback(I2C2_SLAVE_RECEIVED_DATA_DETECTED);
                    }
                    else
                    {
                        // overflow detected!
                        // read the buffer to reset the buffer full flag
                        // and clear the overflow bit
                        // then do nothing so the master
                        // will resend the data
                        dummy = I2C2_RECEIVE_REG;
                        I2C2_RECEIVE_OVERFLOW_STATUS_BIT = 0;
                    }
                }
            }

            break;

        case S_SLAVE_LOW_BYTE_ADDRESS_DETECT:
            // Note that this state will only get
            // executed when 10-bit address is set

            // we send receive request but we do not actually know
            // if the next one is a data from master since the 
            // next one can be a restart with a transmit request.
            // When that happens, the next state will take care of it.
            // This is just the nature of i2c bus protocol.
            not_busy = I2C2_StatusCallback(I2C2_SLAVE_10BIT_RECEIVE_REQUEST_DETECTED);

            // set this flag to indicate we have
            // full 10-bit address detection
            prior_address_match = true;

            if (not_busy)
            {
                // dummy read is needed
                dummy = I2C2_RECEIVE_REG;
            }

            i2c2_slave_state = S_SLAVE_RECEIVE_MODE;

            break;

        case S_SLAVE_TRANSMIT_MODE:

            // this is the state where an ACK or NACK is expected
            // to occur after the slave has placed data to the
            // transmit register.

            // if the transaction was ACK'ed, more data needs to be sent
            // if the transaction was NACK'ed then we don't need to send
            // more data
            if (I2C2_ACKNOWLEDGE_STATUS_BIT == 0)
            {
                // prepare next data
                I2C2_StatusCallback(I2C2_SLAVE_TRANSMIT_REQUEST_DETECTED);

                // transmit more data
                I2C2_TransmitProcess();
                
            }
            else //if (I2C2_ACKNOWLEDGE_STATUS_BIT == 1)
            {
                // no more data to be sent so we go to idle state
                i2c2_slave_state = S_SLAVE_IDLE;
            }
            break;


        default:
            // should never happen, if we ever get here stay here forever
            while(1);
            break;
    }


    // clear the slave interrupt flag
    IFS3bits.SI2C2IF = 0;

}

void I2C2_ReadPointerSet(uint8_t *p)
{
    p_i2c2_read_pointer = p;
}

void I2C2_WritePointerSet(uint8_t *p)
{
    p_i2c2_write_pointer = p;
}

uint8_t *I2C2_ReadPointerGet(void)
{
    return (p_i2c2_read_pointer);
}

uint8_t *I2C2_WritePointerGet(void)
{
    return (p_i2c2_write_pointer);
}

void I2C2_SlaveAddressMaskSet(
                                uint16_t mask)
{
    I2C2_MASK_REG = mask;
}

void I2C2_SlaveAddressSet(
                                uint16_t address)
{
    if (address > 0x7F)
    {
        // use 10 bit address
        I2C2_10_BIT_ADDRESS_ENABLE_BIT = true;
    }
    else
    {
        // use 7 bit address
        I2C2_10_BIT_ADDRESS_ENABLE_BIT = false;
    }
    i2c2_slave_state = S_SLAVE_IDLE;
    I2C2_ADDRESS_REG = address;

}

inline void __attribute__ ((always_inline)) I2C2_TransmitProcess(void)
{
    // get the data to be transmitted

    // sanity check (to avoid stress)
    if (p_i2c2_read_pointer == NULL)
        return;

    I2C2_TRANSMIT_REG = *p_i2c2_read_pointer;

    // set the SCL clock to be released
    I2C2_RELEASE_SCL_CLOCK_CONTROL_BIT = 1;

}

inline void __attribute__ ((always_inline)) I2C2_ReceiveProcess(void)
{   
    // store the received data 
    
    // sanity check (to avoid stress)
    if (p_i2c2_write_pointer == NULL)
        return;

    *p_i2c2_write_pointer = I2C2_RECEIVE_REG;

}

/* Note: This is an example of the I2C2_StatusCallback()
         implementation. This is an emulated EEPROM Memory
         configured to act as a I2C Slave Device.
         For specific slave device implementation, remove
         or modify this function to the specific slave device
         behavior.
*/

static uint8_t i2c2_slaveWriteData = 0xAA;
static uint8_t i2c2_slaveReadData = 0xAA;

static uint8_t buffer[BUFFER_SIZE];
static uint16_t address;

bool I2C2_StatusCallback(I2C2_SLAVE_DRIVER_STATUS status)
{

    switch (status)
    {
        case I2C2_SLAVE_TRANSMIT_REQUEST_DETECTED:

            // set up the slave driver buffer transmit pointer
            i2c2_slaveReadData = buffer[address];
            if (buffer[address]) {
                buffer[address] = 0x00;
                address = (address + 1) % BUFFER_SIZE;
            }
            
            I2C2_ReadPointerSet(&i2c2_slaveReadData);

            break;

        case I2C2_SLAVE_RECEIVE_REQUEST_DETECTED:

            // set up the slave driver buffer receive pointer
            I2C2_WritePointerSet(&i2c2_slaveWriteData);

            break;

        case I2C2_SLAVE_RECEIVED_DATA_DETECTED:

            // tell Bluetooth to turn the light on/off
            if (i2c2_slaveWriteData == 0x01) {
                Lights_On();
            } else if (i2c2_slaveWriteData == 0x02) {
                Lights_Off();
            } else {
                // something went wrong
            };

            break;

        case I2C2_SLAVE_10BIT_RECEIVE_REQUEST_DETECTED:

            // do nothing; 10-bit addresses are not in use

            break;

        default:
            break;

    }

    return true;

}

bool I2C2_QueueByte(uint8_t byte)
{
    if (buffer[address + 1]) {
        return false;
    }
    buffer[address] = byte;
    buffer[address+1] = 0x00;
    
    return true;
}

bool I2C2_QueueMessage(uint8_t* app, uint16_t app_len,
                       uint8_t* title, uint16_t title_len,
                       uint8_t* subtitle, uint16_t subtitle_len)
{
    if (app_len + title_len + subtitle_len + 3 >= BUFFER_SIZE) {
        return false;
    }
    int i;
    for (i = 0; i < app_len; i++) {
        buffer[(address + i)%BUFFER_SIZE] = *(app + i);
    }
    buffer[(address + i)%BUFFER_SIZE] = 0x3A;
    for (i = 0; i < title_len; i++) {
        buffer[(address + app_len + 1 + i)%BUFFER_SIZE] = *(title + i);
    }
    buffer[(address + app_len + 1 + i)%BUFFER_SIZE] = 0x3A;
    for (i = 0; i < subtitle_len; i++) {
        buffer[(address + app_len + title_len + 2 + i)%BUFFER_SIZE] = *(subtitle + i);
    }
    buffer[(address + app_len + title_len + 2 + i)%BUFFER_SIZE] = 0x00;
    
    return true;
}

