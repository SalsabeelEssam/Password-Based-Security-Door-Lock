 /******************************************************************************
 *
 * Module: I2C(TWI)
 * File Name: i2c.h
 * Description: Header file for the I2C(TWI) AVR driver
 *
 * Note		  :	- make structure from type I2C_ConfigType
 * 				to configure I2C settings
 * 				{ SlaveAddress, ClockRate, GCRecognition_Enable, Interrupt}
 *
 * 			 	- F_CPU must be greater than or equal to 8Mhz
 *
 * I2C FRAME Write :
 * 				- TWI_start()  					-> TWI_getStatus() == TW_START
 * 				- TWI_write(Slave_Address)		-> TWI_getStatus() == TW_MT_SLA_W_ACK
 * 				- TWI_write(Data)				-> TWI_getStatus() == TW_MT_SLA_W_ACK
 * 				- TWI_stop()
 * I2C FRAME Read :
 * 				- TWI_start()  					-> TWI_getStatus() == TW_START
 * 				- TWI_write(Slave_Address)		-> TWI_getStatus() == TW_MT_SLA_W_ACK
 * 				- TWI_readWithACK()				-> TWI_getStatus() == TW_MR_DATA_ACK
 * 				- TWI_stop()
 *
 * IF Combined Msg : changed from Write To Read
 * 					 TWI_start() to send repeated start bit , and Read Mode bit
 * 					 				-> TWI_getStatus() == TW_REP_START
 *
 * Author: Mohsen Moawad
 *
 *******************************************************************************/


#ifndef I2C_H_
#define I2C_H_

#include "micro_config.h"
#include "std_types.h"
#include "common_macros.h"

/*******************************************************************************
 *                         Types Declaration                                   *
 *******************************************************************************/
typedef enum{
	DISABLE,ENABLE
}I2C_bool;

typedef enum{
	F_400K,F_100K
}I2C_clock;

/*
 * Initialization:
 * { SlaveAddress, ClockRate, GCRecognition_Enable, Interrupt}
 */
typedef struct{

	/* Set 7-bit Slave address */
	uint8 s_SlaveAddress ;

	/* set Clock of I2C */
	/* F_CPU must be greater than or equal to 8Mhz */
	I2C_clock s_Clock ;						/* (F_400K,F_100K) */

	/* Set General Call Recognition  */
	I2C_bool s_GCRecognition_Enable ;		/* (DISABLE,ENABLE) */

	I2C_bool s_Interrupt ;					/* (DISABLE,ENABLE) */


}I2C_ConfigType;


/*******************************************************************************
 *                       External Variables                                    *
 *******************************************************************************/

/* Extern Public global variables to be used by other modules */

extern volatile uint8 g_i2cData;


/*******************************************************************************
 *                      Preprocessor Macros                                    *
 *******************************************************************************/
/* I2C Status Bits in the TWSR Register */
#define TW_START         	0x08 /* start has been sent */
#define TW_REP_START    	0x10 /* repeated start */

#define TW_MT_SLA_W_ACK  	0x18 /* Master transmit ( slave address + Write request ) to slave + Ack received from slave */
#define TW_MT_SLA_R_ACK  	0x40 /* Master transmit ( slave address + Read request ) to slave + Ack received from slave */
#define TW_MT_DATA_ACK   	0x28 /* Master transmit ( data ) and ACK has been received from Slave. */

#define TW_MR_DATA_ACK   	0x50 /* Master received ( data ) and send ACK to slave */
#define TW_MR_DATA_NACK  	0x58 /* Master received ( data ) but doesn't send ACK to slave */

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description : Function to initialize the I2C driver
 * 	1. Set the Slave Address.
 * 	2. Set the required clock.
 * 	3. Set General Call Recognition
 * 	4. Enable the I2C Interrupt.
 */
void TWI_init(I2C_ConfigType *a_Config_ptr);

/*
 * Description: Function to Send Start Bit on Bus.
 */
void TWI_start(void);

/*
 * Description: Function to Send Stop Bit on Bus.
 */
void TWI_stop(void);

/*
 * Description: Function to Write Data (8-bits) From Master To Slave.
 */
void TWI_write(uint8 data);

/*
 * Description: Function to Read Data (8-bits) from Slave and Master send ACK
 */
uint8 TWI_readWithACK(void);

/*
 * Description: Function to Read Data (8-bits) from Slave and Master send NOT ACK
 */
uint8 TWI_readWithNACK(void);

/*
 * Description: Function to read the Status of TWI Module
 * returns:
 * 			TW_START         	0x08 // start has been sent
 * 			TW_REP_START    	0x10 // repeated start
 * 			TW_MT_SLA_W_ACK  	0x18 // Master transmit ( slave address + Write request ) to slave + Ack received from slave
 * 			TW_MT_SLA_W_NACK  	0x20 // Master transmit ( slave address + Write request ) to slave + NAck received from slave
 * 			TW_MT_SLA_R_ACK  	0x40 // Master transmit ( slave address + Read request ) to slave + Ack received from slave
 * 			TW_MT_DATA_ACK   	0x28 // Master transmit data and ACK has been received from Slave.
 * 			TW_MR_DATA_ACK   	0x50 // Master received data and send ACK to slave
 * 			TW_MR_DATA_NACK  	0x58 // Master received data but doesn't send ACK to slave
 */
uint8 TWI_getStatus(void);

/*
 * Description: Function to set the Call Back function address.
 */
void I2C_setCallBack(void(*a_ptr)(void));



#endif /* I2C_H_ */
