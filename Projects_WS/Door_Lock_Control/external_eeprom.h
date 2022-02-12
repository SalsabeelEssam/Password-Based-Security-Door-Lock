 /******************************************************************************
 *
 * Module: External EEPROM (I2C)
 * File Name:external_eeprom.c
 * Description: Header file for External EEPROM Memory (I2C)
 * Author: Mohsen Moawad
 *
 *******************************************************************************/


#ifndef EXTERNAL_EEPROM_H_
#define EXTERNAL_EEPROM_H_

#include "std_types.h"

/*******************************************************************************
 *                      Preprocessor Macros                                    *
 *******************************************************************************/
#define ERROR 0
#define SUCCESS 1
#define EEPROM_FIXED_ADDRESS 0xA0

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description: Function to Initialize External EEPROM .
 */
void EEPROM_init(void);

/*
 * Description: Function to write Data(8-bits) into EEPROM address (16-bit) .
 */
uint8 EEPROM_writeByte(uint16 u16addr,uint8 u8data);

/*
 * Description: Function to Read Data(8-bits) From EEPROM address (16-bit) .
 */
uint8 EEPROM_readByte(uint16 u16addr,uint8 *u8data);
 
#endif /* EXTERNAL_EEPROM_H_ */
