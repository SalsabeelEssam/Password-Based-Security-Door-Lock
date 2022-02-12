 /******************************************************************************
 *
 * Module: 		LCD
 * File Name: 	lcd.h
 * Description: Header file for the LCD driver
 * Author: 		Mohsen Moawad
 *
 *******************************************************************************/

#ifndef LCD_H_
#define LCD_H_

#include "std_types.h"
#include "common_macros.h"
#include "micro_config.h"

/*******************************************************************************
 *                 Preprocessor Macros Configuration                           *
 *******************************************************************************/

/* LCD Data bits mode configuration */
#define DATA_BITS_MODE 8

/* Use higher 4 bits in the data port */
#if (DATA_BITS_MODE == 4)
#define UPPER_PORT_PINS
#endif

/* LCD HW Pins */
#define RS 								PD5
#define RW 								PD6
#define E  								PD7
#define LCD_CTRL_PORT 					PORTD
#define LCD_CTRL_PORT_DIR			 	DDRD
#define LCD_DATA_PORT 					PORTB
#define LCD_DATA_PORT_DIR				DDRB


/*******************************************************************************
 *                      Preprocessor Macros                                    *
 *******************************************************************************/

/* LCD Commands */
#define CLEAR_COMMAND 					0x01
#define TWO_LINE_LCD_Eight_BIT_MODE 	0x38
#define TWO_LINE_LCD_FOUR_BIT_MODE 		0x28
#define RETURN_HOME						0x02
#define CURSOR_OFF 						0x0C
#define CURSOR_ON 						0x0E
#define SET_CURSOR_LOCATION 			0x80
#define SHIF_DISPLAY_RIGH				0x05
#define SHIF_DISPLAY_LEFT				0x07
#define CURSOR_BLINK					0x0F

/* LCD Addresses */
#define FIRST_ROW						0x00
#define SECOND_ROW						0x40
#define THIRD_ROW						0x10
#define FORTH_ROW						0x50

/* Numbers Base */
#define DECIMAL 						(10)
#define HEXA							(16)
#define OCTA							(8)
#define BINARY							(2)

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description: Function to send a command to LCD
 * 				in Hexa form from Datasheet or instructions set in lcd.h
 */
void LCD_sendCommand(uint8 a_command);

/*
 * Description: Function to send and display a character on LCD
 */
void LCD_displayCharacter(uint8 a_data);

/*
 * Description: Function to display string on LCD
 */
void LCD_displayString(const char *Str_ptr);

/*
 * Description: Function to initialize LCD after select LCD pins in lcd.h file
 */
void LCD_init(void);

/*
 * Description: Function to clear screen
 */
void LCD_clearScreen(void);

/*
 * Description: Function to display string at specific row and column
 */
void LCD_displayStringRowColumn(uint8 a_row,uint8 a_col,const char *Str_ptr);

/*
 * Description: Function to set Cursor at specific row and column
 */
void LCD_goToRowColumn(uint8 a_row,uint8 a_col);

/*
 * Description: Function to Display integer values as String
 */
void LCD_intgerToString(int a_data,int a_numberBase);

/*
 * Description: Function to Convert integer to ASCII from stdlib
 */
void itoa(int value, char* str, int base);

#endif /* LCD_H_ */
