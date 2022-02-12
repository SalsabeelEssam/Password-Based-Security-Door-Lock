 /******************************************************************************
 *
 * Module: 		UART
 * File Name: 	uart.h
 * Description: Header file for the AVR UART driver
 * Author: 		Mohsen Moawad
 *
 *******************************************************************************/

#ifndef UART_H_
#define UART_H_

#include "micro_config.h"
#include "std_types.h"
#include "common_macros.h"

/*******************************************************************************
 *                         Types Declaration                                   *
 *******************************************************************************/
typedef enum
{
	DISABLE_INT,ENABLE_INT
}UART_Interrupt;

typedef enum
{
	DISABLE = 0, EVEN = 2, ODD = 3
}UART_Parity;

typedef enum
{
	OneBit,TwoBit
}UART_StopBit;

typedef enum
{
	 BR2400 = 2400 , BR4800 = 4800 , BR9600 = 9600 , BR115200 = 115200
}UART_BaudRate;


typedef struct
{
	/****************** Bits For UCSRB Register ************************/
	/* RX Complete Interrupt Enable */
	UART_Interrupt s_RxInterruptEnable;				/* DISABLE , ENABLE */
	/* TX Complete Interrupt Enable */
	UART_Interrupt s_TxInterruptEnable;				/* DISABLE , ENABLE */
	/* USART Data Register Empty Interrupt Enable */
	UART_Interrupt s_DataRegEmptyInterruptEnable;	/* DISABLE , ENABLE */

	/****************** Bits For UCSRC Register ************************/
	/* Choose Parity Mode*/
	UART_Parity s_Parity;	/* DISABLE , EVEN , ODD */
	/* Select the number of Stop Bits */
	UART_StopBit s_Stop;	/* OneBit , TwoBit */

	/****************** Bits For UBRR Register *************************/
	/* Choose UART Baud Rate from (2400,4800,9600,115200) */
	UART_BaudRate s_BaudRate;	/* BR2400 , BR4800 , BR9600 , BR115200 */

	/************* Choose NULL Terminator Character  *******************/
	/* Choose NULL Terminator for Receiving String Function.
	 * Send the NULL Terminator'#' twice at the end of The String in
	 * UART_sendString Function if RX Interrupt Enabled .
	 * Or send it Once if RX Interrupt Disabled. */

	uint8 s_NULL_Terminator ;	/* ( '#' , '%' , '&' , ... ) */

}UART_ConfigType;


/*******************************************************************************
 *                      Preprocessor Macros                                    *
 *******************************************************************************/

#define InterruptIsEnbale(BIT) ( UCSRB & (1<<BIT) )
#define RxInterrupt RXCIE
#define TxInterrupt TXCIE
#define DREInterrupt UDRIE

/*******************************************************************************
 *                       External Variables                                    *
 *******************************************************************************/

/* Extern Public global variables to be used by other modules */

extern volatile uint16 g_uartData ;

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
/*
 * Description :
 * 	Function to initialize the UART driver
 * 		1. Enables the Interrupts .
 * 		2. Set the Parity Mode.
 *	 	3. Set The Number of Stop Bit.
 * 		4. Select Baud Rate
 * 		5. Set Custom NULL Terminator
 */
void UART_init(const UART_ConfigType * Config_ptr);

/*
 * Description:
 * Function responsible for Sending Byte
 *
 * Arguments:
 * uint8 Data
 */
void UART_sendByte(const uint8 data);

/*
 * Description:
 * Function responsible for Receiving Byte
 *
 * Return:
 * uint8 Data
 */
uint8 UART_receiveByte(void);

/*
 * Description:
 * Function responsible for Sending String Over UART
 *
 * Arguments:
 * address of the Array which will be sent
 *
 * Note:
 * 		- if RX Interrupt Enabled
 * Send the NULL Terminator '#' twice at the end of The String in
 * UART_sendString Function.
 *
 * 		- Or send it Once if RX Interrupt Disabled. */
void UART_sendString(const uint8 *Str);

/*
 * Description:
 * Function responsible for Receiving String Over UART
 * Receive until NULL Terminator '#'
 *
 * Arguments:
 * 	Array address to Store The Data
 */
void UART_receiveString(uint8 *Str);


/*
 * Description:
 * Function to set the Call Back function address For RX ISR.
 */
void UART_RXC_setCallBack(void(*a_ptr)(void));

/*
 * Description:
 * Function to set the Call Back function address For TX ISR.
 */
void UART_TXC_setCallBack(void(*a_ptr)(void));

/*
 * Description:
 * Function to set the Call Back function address DRE ISR.
 */
void UART_UDRE_setCallBack(void(*a_ptr)(void));

#endif /* UART_H_ */
