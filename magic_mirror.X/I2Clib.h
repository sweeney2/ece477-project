/* 
 * File:   I2Clib.h
 * Author: alexlagerman
 *
 * Created on November 9, 2016, 11:45 AM
 */

#ifndef I2CLIB_H
#define	I2CLIB_H

#include "p24fj128ga010.h"
#include <GenericTypeDefs.h>

//--------------------Constants--------------------
#define I2C_M_ACK	0
#define I2C_M_NACK	1

//--------------------Structs--------------------
//--------------------Variables--------------------
//--------------------Functions--------------------
extern void I2C_HWini(void);
extern void I2C_ModuleStart(void);
extern void I2C_SWini(void);
extern int  I2C1_M_BusReset(void);
extern void I2C1_M_ClearErrors(void);
extern int  I2C1_M_Poll(BYTE);
extern int  I2C1_M_RcvByte(BYTE);
extern int  I2C1_M_RecoverBus(void);
extern int  I2C1_M_Read(BYTE, BYTE, int, char *);
extern int  I2C1_M_ReStart(void);
extern int  I2C1_M_SendByte(char);
extern int  I2C1_M_Start(void);
extern int  I2C1_M_Stop(void);
extern void I2C1_M_Write(BYTE, BYTE, int, char *);

//-------------------Macros-------------------
#define SetI2C1BusDirty	I2Cflags |=  0x0001
#define ClrI2C1BusDirty	I2Cflags &= ~0x0001
#define IsI2C1BusDirty	(I2Cflags & 0x0001)
#ifdef	__cplusplus
extern "C" {
#endif




#ifdef	__cplusplus
}
#endif

#endif	/* I2CLIB_H */

