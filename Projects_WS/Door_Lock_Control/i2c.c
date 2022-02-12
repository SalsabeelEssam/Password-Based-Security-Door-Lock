 /******************************************************************************
 *
 * Module: I2C(TWI)
 * File Name: i2c.c
 * Description: Source file for the I2C AVR driver
 * Author: Mohsen Moawad
 *
 *******************************************************************************/
 
#include "i2c.h"

/*******************************************************************************
 *                          Global Variables                                   *
 *******************************************************************************/

/* Global Variable for I2C Interrupt , to use it in all function */
static bool g_i2cInterrupt ;

/* Global Variable to store Data */
volatile uint8 g_i2cData;

/* g_I2C_callBack_ptr Global Pointer to function to store I2C Call Back Function */
static void (*g_I2C_callBack_ptr)(void) = NULL_PTR ;

/*******************************************************************************
 *                          ISR's Definitions                                  *
 *******************************************************************************/

ISR(TWI_vect)
{

	g_i2cData = TWDR ;
	if(g_I2C_callBack_ptr != NULL_PTR)
	{
		/* Call the Call Back function in the application after the I2C finished */
		(*g_I2C_callBack_ptr)();
	}
}




/*******************************************************************************
 *                          Functions Definitions                              *
 *******************************************************************************/

/*
 * Description : Function to initialize the I2C driver
 * 	1. Set the required clock.
 * 	2. Set the Slave Address.
 * 	3. Set General Call Recognition
 * 	4. Enable the I2C Interrupt.
 */
void TWI_init(I2C_ConfigType *a_Config_ptr)
{
	/* make a global pointer to the Configuration Structure */
	g_i2cInterrupt = a_Config_ptr->s_Interrupt ;

    /* Bit Rate: 400.000 kbps or 100.000 kbps using zero pre-scaler TWPS=00 and F_CPU=8Mhz */
    /* SCL_Freq = F_CPU / ( 16 + 2(TWBR) * 4^TWPS ) */

    if(a_Config_ptr->s_Clock == F_400K && F_CPU >= 8000000)
    {
    	TWBR = ((uint8)(((F_CPU/400000) -16) / 2)) ;		/* TWBR = 2 for F_CPU=8Mhz */
    	TWSR = 0x00;
    }
    else if(a_Config_ptr->s_Clock == F_100K && F_CPU >= 8000000)
    {
    	TWBR = ((uint8)(((F_CPU/100000) -16) / 2)) ;		/* TWBR = 32 for F_CPU=8Mhz */
    	TWSR = 0x00;
    }
	
	 /* Two Wire Bus address my address if any master device want to call me: s_SlaveAddress
	  * (used in case this MC is a slave device)
	  * General Call Recognition: s_GCRecognition_Enable */
	TWAR = ((TWAR & (0x01)) | ((a_Config_ptr->s_SlaveAddress) << TWA0 ));
	TWAR = ((TWAR & (0xFE)) | ((a_Config_ptr->s_GCRecognition_Enable) << TWGCE ));
	
	/* Set Interrupt */
	TWCR = (TWCR & ~(1<<TWIE)) | ((a_Config_ptr->s_Interrupt) << TWIE);

	/* Enable TWI Module */
    TWCR |= (1<<TWEN);
}

/*
 * Description: Function to Send Start Bit on Bus.
 */
void TWI_start(void)
{
    /* 
	 * Clear the TWINT flag before sending the start bit TWINT=1
	 * send the start bit by TWSTA=1
	 * Enable TWI Module TWEN=1 
	 */
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    
    /* Wait for TWINT flag set in TWCR Register (start bit is send successfully) */
    while(BIT_IS_CLEAR(TWCR,TWINT));
}

/*
 * Description: Function to Send Stop Bit on Bus.
 */
void TWI_stop(void)
{
    /* 
	 * Clear the TWINT flag before sending the stop bit TWINT=1
	 * send the stop bit by TWSTO=1
	 * Enable TWI Module TWEN=1 
	 */
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}

/*
 * Description: Function to Write Data (8-bits) From Master To Slave.
 */
void TWI_write(uint8 data)
{
	if(g_i2cInterrupt)
	{
	    /* Put data On TWI data Register */
	    TWDR = data;
	    /*
		 * Clear the TWINT flag before sending the data TWINT=1
		 * Enable TWI Module TWEN=1
		 */
	    TWCR = (1 << TWINT) | (1 << TWEN);
	    /* Wait for TWINT flag set in TWCR Register(data is send successfully) */
	    while(BIT_IS_CLEAR(TWCR,TWINT));
	}
	else
	{
	    /* Put data On TWI data Register */
	    TWDR = data;
	    /*
		 * Clear the TWINT flag before sending the data TWINT=1
		 * Enable TWI Module TWEN=1
		 */
	    TWCR = (1 << TWINT) | (1 << TWEN);
	    /* Wait for TWINT flag set in TWCR Register(data is send successfully) */
	    while(BIT_IS_CLEAR(TWCR,TWINT));
	}

}

/*
 * Description: Function to Read Data (8-bits) from Slave and Master send ACK
 */
uint8 TWI_readWithACK(void)
{
	if(g_i2cInterrupt)
	{
		/*
		 * Clear the TWINT flag before reading the data TWINT=1
		 * Enable sending ACK after reading or receiving data TWEA=1
		 * Enable TWI Module TWEN=1
		 */
	    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
	    /* Wait for TWINT flag set in TWCR Register (data received successfully) */
	    while(BIT_IS_CLEAR(TWCR,TWINT));
	    /* Read Data */
	    return TWDR;
	}
	else
	{
		/*
		 * Clear the TWINT flag before reading the data TWINT=1
		 * Enable sending ACK after reading or receiving data TWEA=1
		 * Enable TWI Module TWEN=1
		 */
	    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
	    /* Wait for TWINT flag set in TWCR Register (data received successfully) */
	    while(BIT_IS_CLEAR(TWCR,TWINT));
	    /* Read Data */
	    return TWDR;
	}
}

/*
 * Description: Function to Read Data (8-bits) from Slave and Master send NOT ACK
 */
uint8 TWI_readWithNACK(void)
{
	if(g_i2cInterrupt)
	{
		/*
		 * Clear the TWINT flag before reading the data TWINT=1
		 * Enable TWI Module TWEN=1
		 */
		TWCR = (1 << TWINT) | (1 << TWEN);
		/* Wait for TWINT flag set in TWCR Register (data received successfully) */
		while(BIT_IS_CLEAR(TWCR,TWINT));
		/* Read Data */
		return TWDR;
	}
	else
	{
		/*
		 * Clear the TWINT flag before reading the data TWINT=1
		 * Enable TWI Module TWEN=1
		 */
	    TWCR = (1 << TWINT) | (1 << TWEN);
	    /* Wait for TWINT flag set in TWCR Register (data received successfully) */
	    while(BIT_IS_CLEAR(TWCR,TWINT));
	    /* Read Data */
	    return TWDR;
	}

}

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
uint8 TWI_getStatus(void)
{
    uint8 status;
    /* masking to eliminate first 3 bits and get the last 5 bits (status bits) */
    status = TWSR & 0xF8;
    return status;
}

/*
 * Description: Function to set the Call Back function address.
 */
void I2C_setCallBack(void(*a_ptr)(void))
{
	/* Save the address of the Call back function in a global variable */
	g_I2C_callBack_ptr = a_ptr ;

}
