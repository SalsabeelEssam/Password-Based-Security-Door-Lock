 /******************************************************************************
 *
 * Module: 		LCD
 * File Name: 	lcd.c
 * Description: Source file for the LCD driver
 * Author: 		Mohsen Moawad
 *
 *******************************************************************************/

#include "lcd.h"

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

/*
 * Description: Function to initialize LCD after select LCD pins in lcd.h file
 */
void LCD_init(void)
{
	/* Configure the control pins(E,RS,RW) as output pins */
	LCD_CTRL_PORT_DIR |= (1<<E) | (1<<RS) | (1<<RW);

#if (DATA_BITS_MODE == 4)
	#ifdef UPPER_PORT_PINS
		/* Configure the highest 4 bits of the data port as output pins */
		LCD_DATA_PORT_DIR |= 0xF0 ;
	#else
		/* Configure the lowest 4 bits of the data port as output pins */
		LCD_DATA_PORT_DIR |= 0x0F ;
	#endif
	/* to work with 4-bit Data Mode , send command Return Home */
	LCD_sendCommand(RETURN_HOME);
	/* use 2-line LCD + 4-bit Data Mode + 5*7 dot display Mode */
	LCD_sendCommand(TWO_LINE_LCD_FOUR_BIT_MODE);
#elif (DATA_BITS_MODE == 8)
	/* Configure the data port as output port */
	LCD_DATA_PORT_DIR = 0xFF;
	/* use 2-line LCD + 8-bit Data Mode + 5*7 dot display Mode */
	LCD_sendCommand(TWO_LINE_LCD_Eight_BIT_MODE);
#endif

	LCD_sendCommand(CURSOR_OFF); /* cursor off */
	LCD_sendCommand(CLEAR_COMMAND); /* clear LCD at the beginning */
}

/*
 * Description: Function to send a command to LCD
 * 				in Hexa form from Datasheet or instructions set in lcd.h
 */
void LCD_sendCommand(uint8 a_command)
{
	/* commands flow from AC Characteristics in Datasheet */

	CLEAR_BIT(LCD_CTRL_PORT,RS); /* Instruction Mode RS=0 */
	CLEAR_BIT(LCD_CTRL_PORT,RW); /* write data to LCD so RW=0 */

/*********************** Sending Command  ****************************/

	_delay_ms(1); /* delay for processing Tas = 50ns */
	SET_BIT(LCD_CTRL_PORT,E); /* Enable LCD E=1 */
	_delay_ms(1); /* delay for processing Tpw - Tdws = 190ns */

	/* Commands flow with 4-bits Mode*/
#if (DATA_BITS_MODE == 4)
	/* out the highest 4 bits of the required command to the data bus D4 --> D7
	 *
	 * Ex : command 0x28 (0010 1000 )
	 * send ( 0010 ) first
	 * */
	#ifdef UPPER_PORT_PINS
	/*  LCD_DATA_PORT = ( 0000 xxxx ) 		   | ( 0100 0000 )     */
		LCD_DATA_PORT = (LCD_DATA_PORT & 0x0F) | (a_command & 0xF0) ;
	#else
	/*  LCD_DATA_PORT = ( xxxx 0000 ) 	       | (( 0100 0000 ) >> 4) --> (0000 0100)  */
		LCD_DATA_PORT = (LCD_DATA_PORT & 0xF0) | ((a_command & 0xF0) >> 4 ) ;
	#endif
	_delay_ms(1); /* delay for processing Tdsw = 100ns */
	CLEAR_BIT(LCD_CTRL_PORT,E); /* disable LCD E=0 */
	_delay_ms(1); /* delay for processing Th = 13ns */

/*********************** Sending Command DONE  ***********************/
/*************************** Sending Command  ************************/

	SET_BIT(LCD_CTRL_PORT,E); /* Enable LCD E=1 */
	_delay_ms(1); /* delay for processing Tpw - Tdws = 190ns */

	/* out the lowest 4 bits of the required command to the data bus D4 --> D7
	 *
	 * Ex : command 0x28 (0010 1000 )
	 * send ( 1000 )
	 * */
	#ifdef UPPER_PORT_PINS
	/*  LCD_DATA_PORT = ( 0000 xxxx ) 		   | ( 0000 1000 ) << 4) --> (1000 0000)  */
		LCD_DATA_PORT = (LCD_DATA_PORT & 0x0F) | ((a_command & 0x0F) << 4 ) ;
	#else
	/*  LCD_DATA_PORT = ( xxxx 0000 ) 	       | (( 0000 1000 ) */
		LCD_DATA_PORT = (LCD_DATA_PORT & 0xF0) | (a_command & 0x0F) ;
	#endif

	_delay_ms(1); /* delay for processing Tdsw = 100ns */
	CLEAR_BIT(LCD_CTRL_PORT,E); /* disable LCD E=0 */
	_delay_ms(1); /* delay for processing Th = 13ns */

/*********************** Sending Command DONE  ***********************/

	/* Commands flow with 8-bits Mode*/
#elif (DATA_BITS_MODE == 8)

	LCD_DATA_PORT = a_command; /* out the required command to the data bus D0 --> D7 */
	_delay_ms(1); /* delay for processing Tdsw = 100ns */
	CLEAR_BIT(LCD_CTRL_PORT,E); /* disable LCD E=0 */
	_delay_ms(1); /* delay for processing Th = 13ns */
#endif

}

/*
 * Description: Function to send and display a character on LCD
 */
void LCD_displayCharacter(uint8 a_data)
{
	/* commands flow from AC Characteristics in Datasheet */

	SET_BIT(LCD_CTRL_PORT,RS); /* Data Mode RS=1 */
	CLEAR_BIT(LCD_CTRL_PORT,RW); /* write data to LCD so RW=0 */

/*********************** Sending Data  ****************************/

	_delay_ms(1); /* delay for processing Tas = 50ns */
	SET_BIT(LCD_CTRL_PORT,E); /* Enable LCD E=1 */
	_delay_ms(1); /* delay for processing Tpw - Tdws = 190ns */

	/* Data flow with 4-bits Mode*/
#if (DATA_BITS_MODE == 4)
	/* out the highest 4 bits of the required data to the data bus D4 --> D7
	 *
	 * Ex : data 0x28 (0010 1000 )
	 * send ( 0010 ) first
	 * */
	#ifdef UPPER_PORT_PINS
	/*  LCD_DATA_PORT = ( 0000 xxxx ) 		   | ( 0100 0000 )     */
		LCD_DATA_PORT = (LCD_DATA_PORT & 0x0F) | (a_data & 0xF0) ;
	#else
	/*  LCD_DATA_PORT = ( xxxx 0000 ) 	       | (( 0100 0000 ) >> 4) --> (0000 0100)  */
		LCD_DATA_PORT = (LCD_DATA_PORT & 0xF0) | ((a_data & 0xF0) >> 4 ) ;
	#endif
	_delay_ms(1); /* delay for processing Tdsw = 100ns */
	CLEAR_BIT(LCD_CTRL_PORT,E); /* disable LCD E=0 */
	_delay_ms(1); /* delay for processing Th = 13ns */

/*********************** Sending Data DONE  ***********************/
/************************* Sending Data  **************************/

	SET_BIT(LCD_CTRL_PORT,E); /* Enable LCD E=1 */
	_delay_ms(1); /* delay for processing Tpw - Tdws = 190ns */

	/* out the lowest 4 bits of the required data to the data bus D4 --> D7
	 *
	 * Ex : data 0x28 (0010 1000 )
	 * send ( 1000 )
	 * */
	#ifdef UPPER_PORT_PINS
	/*  LCD_DATA_PORT = ( 0000 xxxx ) 		   | ( 0000 1000 ) << 4) --> (1000 0000)  */
		LCD_DATA_PORT = (LCD_DATA_PORT & 0x0F) | ((a_data & 0x0F) << 4 ) ;
	#else
	/*  LCD_DATA_PORT = ( xxxx 0000 ) 	       | (( 0000 1000 ) */
		LCD_DATA_PORT = (LCD_DATA_PORT & 0xF0) | (a_data & 0x0F) ;
	#endif

	_delay_ms(1); /* delay for processing Tdsw = 100ns */
	CLEAR_BIT(LCD_CTRL_PORT,E); /* disable LCD E=0 */
	_delay_ms(1); /* delay for processing Th = 13ns */

/*********************** Sending Data DONE  ***********************/

	/* Commands flow with 8-bits Mode*/
#elif (DATA_BITS_MODE == 8)

	LCD_DATA_PORT = a_data; /* out the required data to the data bus D0 --> D7 */
	_delay_ms(1); /* delay for processing Tdsw = 100ns */
	CLEAR_BIT(LCD_CTRL_PORT,E); /* disable LCD E=0 */
	_delay_ms(1); /* delay for processing Th = 13ns */
#endif
}

/*
 * Description: Function to display string on LCD
 */
void LCD_displayString(const char *Str_ptr)
{
	while((*Str_ptr) != '\0')
	{
		/* Send String Character by Character */
		LCD_displayCharacter(*Str_ptr);
		Str_ptr++;
	}		
}

/*
 * Description: Function to set Cursor at specific row and column
 */
void LCD_goToRowColumn(uint8 a_row,uint8 a_col)
{
	uint8 a_address;
	
	/* first of all calculate the required address */
	switch(a_row)
	{
		case 0:
			a_address = a_col + FIRST_ROW;
				break;
		case 1:
			a_address = a_col + SECOND_ROW;
				break;
		case 2:
			a_address = a_col + THIRD_ROW;
				break;
		case 3:
			a_address = a_col + FORTH_ROW;
				break;
	}					
	/* to write to a specific address in the LCD 
	 * we need to apply the corresponding command 0b10000000+Address */
	LCD_sendCommand(a_address | SET_CURSOR_LOCATION);
}

/*
 * Description: Function to display string at specific row and column
 */
void LCD_displayStringRowColumn(uint8 a_row,uint8 a_col,const char *Str_ptr)
{
	LCD_goToRowColumn(a_row,a_col); /* go to to the required LCD position */
	LCD_displayString(Str_ptr); /* display the string */
}

/*
 * Description: Function to Display integer values as String
 */
void LCD_intgerToString(int a_data,int a_numberBase)
{
   char buff[16]; /* String to hold the ascii result */
   itoa(a_data,buff,a_numberBase); /* 10 for decimal */
   LCD_displayString(buff);
}

/*
 * Description: Function to clear screen
 */
void LCD_clearScreen(void)
{
	LCD_sendCommand(CLEAR_COMMAND); //clear display 
}
