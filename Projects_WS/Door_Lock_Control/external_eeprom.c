
 /******************************************************************************
 *
 * Module: External EEPROM (I2C)
 * File Name:external_eeprom.c
 * Description: Source file for External EEPROM Memory (I2C)
 * Author: Mohsen Moawad
 *
 *******************************************************************************/
#include "i2c.h"
#include "external_eeprom.h"

/*
 * Description: Function to Initialize External EEPROM .
 */
void EEPROM_init(void)
{
	/* Create configuration structure for I2C driver */
	I2C_ConfigType I2C_Config = {.s_SlaveAddress = 0x01 , .s_Clock = F_400K} ;

	/* initialize I2C(TWI) module inside the MC */
	TWI_init(&I2C_Config);
}

/*
 * Description: Function to write Data(8-bits) into EEPROM in address (16-bit) .
 */
uint8 EEPROM_writeByte(uint16 u16addr, uint8 u8data)
{
	/* Send the Start Bit */
    TWI_start();
    if (TWI_getStatus() != TW_START)
        return ERROR;
		
    /* Send the device address, we need to get A8 A9 A10 address bits from the
     * memory location address and R/W=0 (write) */
    TWI_write((uint8)((EEPROM_FIXED_ADDRESS) | ((u16addr & 0x0700)>>7)));
    if (TWI_getStatus() != TW_MT_SLA_W_ACK)
        return ERROR; 
		 
    /* Send the required memory location address
     * only Least 8 bits A0:A7 */
    TWI_write((uint8)(u16addr));
    if (TWI_getStatus() != TW_MT_DATA_ACK)
        return ERROR;
		
    /* write byte to eeprom */
    TWI_write(u8data);
    if (TWI_getStatus() != TW_MT_DATA_ACK)
        return ERROR;

    /* Send the Stop Bit */
    TWI_stop();
	
    return SUCCESS;
}


/*
 * Description: Function to Read Data(8-bits) From EEPROM address (16-bit) .
 */
uint8 EEPROM_readByte(uint16 u16addr, uint8 *u8data)
{
	/* Send the Start Bit */
    TWI_start();
    if (TWI_getStatus() != TW_START)
        return ERROR;
		
    /* Send the device address, we need to get A8 A9 A10 address bits from the
     * memory location address and R/W=0 (write) */
    TWI_write((uint8)((EEPROM_FIXED_ADDRESS) | ((u16addr & 0x0700)>>7)));
    if (TWI_getStatus() != TW_MT_SLA_W_ACK)
        return ERROR;
		
    /* Send the required memory location address
     * only Least 8 bits A0:A7 */
    TWI_write((uint8)(u16addr));
    if (TWI_getStatus() != TW_MT_DATA_ACK)
        return ERROR;
		
    /* Send the Repeated Start Bit */
    TWI_start();
    if (TWI_getStatus() != TW_REP_START)
        return ERROR;
		
    /* Send the device address, we need to get A8 A9 A10 address bits from the
     * memory location address and R/W=1 (Read) */
    TWI_write((uint8)((EEPROM_FIXED_ADDRESS) | ((u16addr & 0x0700)>>7) | 1 /* R/W bit */));
    if (TWI_getStatus() != TW_MT_SLA_R_ACK)
        return ERROR;

    /* Read Byte from Memory without send ACK */
    *u8data = TWI_readWithNACK();
    if (TWI_getStatus() != TW_MR_DATA_NACK)
        return ERROR;

    /* Send the Stop Bit */
    TWI_stop();
    return SUCCESS;
}
