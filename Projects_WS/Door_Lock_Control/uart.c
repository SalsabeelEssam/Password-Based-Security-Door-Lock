 /******************************************************************************
 *
 * Module: 		UART
 * File Name: 	uart.c
 * Description: Source file for the AVR UART driver
 * Author: 		Mohsen Moawad
 *
 *******************************************************************************/

#include "uart.h"

/*******************************************************************************
 *                          Global Variables                                   *
 *******************************************************************************/
/* g_uartData Global Variable to store UART Data and use it in Any Project */
volatile uint16 g_uartData = 0;

static void (*g_UART_RXC_callBack_ptr)(void) = NULL_PTR ;
static void (*g_UART_TXC_callBack_ptr)(void) = NULL_PTR ;
static void (*g_UART_UDRE_callBack_ptr)(void) = NULL_PTR ;

static uint8 g_NULL_Terminator = '#' ;

/*******************************************************************************
 *                          ISR's Definitions                                  *
 *******************************************************************************/

ISR(USART_TXC_vect)
{

	if(g_UART_TXC_callBack_ptr != NULL_PTR)
	{
		/* Call the Call Back function in the application after the UART Tx Complete */
		(*g_UART_TXC_callBack_ptr)();
	}
}

ISR(USART_RXC_vect)
{
	g_uartData = UDR ;
	if(g_UART_RXC_callBack_ptr != NULL_PTR)
	{
		/* Call the Call Back function in the application after the UART Rx Complete */
		(*g_UART_RXC_callBack_ptr)();
	}
}

ISR(USART_UDRE_vect)
{
	if(g_UART_UDRE_callBack_ptr != NULL_PTR)
	{
		/* Call the Call Back function in the application when Data Register Empty */
		(*g_UART_UDRE_callBack_ptr)();
	}
}

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/
void UART_init(const UART_ConfigType * Config_ptr)
{
	/* U2X = 1 for double transmission speed */
	UCSRA = (1<<U2X);

	/************************** UCSRB Description **************************
	 * RXCIE = USART RX Complete Interrupt Enable
	 * TXCIE = USART Tx Complete Interrupt Enable
	 * UDRIE = USART Data Register Empty Interrupt Enable
	 * RXEN  = 1 Receiver Enable
	 * RXEN  = 1 Transmitter Enable
	 * UCSZ2 = 0 For 8-bit data mode
	 * RXB8 & TXB8 not used for 8-bit data mode
	 ***********************************************************************/ 
	UCSRB = (UCSRB & 0x1F) |  (1<<RXEN) | (1<<TXEN)
			| (Config_ptr->s_RxInterruptEnable << RXCIE)
			| (Config_ptr->s_TxInterruptEnable << TXCIE)
			| (Config_ptr->s_DataRegEmptyInterruptEnable << UDRIE);
	

	/************************** UCSRC Description **************************
	 * URSEL   = 1 The URSEL must be one when writing the UCSRC
	 * UMSEL   = 0 Asynchronous Operation
	 * UPM1:0  = parity bit
	 * USBS    = stop bit
	 * UCSZ1:0 = 11 For 8-bit data mode
	 * UCPOL   = 0 Used with the Synchronous operation only
	 ***********************************************************************/ 	
	UCSRC = ( UCSRC & 0xC7 ) | (1<<URSEL) | (1<<UCSZ0) | (1<<UCSZ1)
			| (Config_ptr->s_Parity << UPM0)
			| (Config_ptr->s_Stop << USBS);
	
	/* First 8 bits from the BAUD_PRESCALE inside UBRRL and last 4 bits in UBRRH*/
	UBRRH = (((F_CPU / (Config_ptr->s_BaudRate * 8UL)) - 1) >> 8 );
	UBRRL = ((F_CPU / (Config_ptr->s_BaudRate * 8UL)) - 1);

	/**************** Set NULL Terminator Character  *******************/
	if (Config_ptr->s_NULL_Terminator != 0 )
		g_NULL_Terminator = Config_ptr->s_NULL_Terminator ;

}
	
void UART_sendByte(const uint8 data)
{
	/* UDRE flag is set when the Tx buffer (UDR) is empty and ready for 
	 * transmitting a new byte so wait until this flag is set to one */
	while(BIT_IS_CLEAR(UCSRA,UDRE)){}
	/* Put the required data in the UDR register and it also clear the UDRE flag as 
	 * the UDR register is not empty now */	 
	UDR = data;
	/************************* Another Method *************************
	UDR = data;
	while(BIT_IS_CLEAR(UCSRA,TXC)){} // Wait until the transmission is complete TXC = 1
	SET_BIT(UCSRA,TXC); // Clear the TXC flag
	*******************************************************************/	
}

uint8 UART_receiveByte(void)
{
	if(InterruptIsEnbale(RxInterrupt))
	{
		return g_uartData ;
	}
	else
	{
		/* RXC flag is set when the UART receive data so wait until this
		 * flag is set to one */
		while(BIT_IS_CLEAR(UCSRA,RXC)){}
		/* Read the received data from the Rx buffer (UDR) and the RXC flag
		   will be cleared after read this data */
		return UDR;
	}
}

void UART_sendString(const uint8 *Str)
{
	uint8 i = 0;
	/* if TX Interrupt Is Enabled */
	if(InterruptIsEnbale(TxInterrupt))
	{
		/* Disable TX Interrupt to avoid calling the callback function repeatedly */
		UCSRB &= (~(1<<TXCIE)) ;
		while(Str[i] != '\0')
		{
			if(Str[i+1] == '\0')
			{
				/* Enable TX Interrupt , To call the callback function
				 * after sending last character next loop */
				UCSRB |= (1<<TXCIE) ;
			}
			UART_sendByte(Str[i]);
			i++;
		}
	}
	/* else if TX Interrupt Is Disable  */
	else
	{
		while(Str[i] != '\0')
		{
			UART_sendByte(Str[i]);
			i++;
		}
	}
}

void UART_receiveString(uint8 *Str)
{
	uint8 i = 0;
	/* If RX Interrupt Enabled */
	if(InterruptIsEnbale(RxInterrupt))
	{
		/* Disable RX Interrupt to avoid calling the callback function repeatedly */
		UCSRB &= (~(1<<RXCIE)) ;

		Str[i] = UART_receiveByte();
		while(Str[i] != g_NULL_Terminator )
		{
			i++;
			Str[i] = UART_receiveByte();
			if(Str[i] == g_NULL_Terminator)
			{
				/* Enable RX Interrupt , To call the callback function
				 * after receiving last character next loop */
				UCSRB |= (1<<RXCIE) ;

				/* receive the duplicated NULL Terminator to set Receiving flag
				 * and fire the ISR*/
				Str[i] = UART_receiveByte();
			}
		}
		Str[i] = '\0';
	}
	/* Else If RX Interrupt Disabled */
	else
	{
		Str[i] = UART_receiveByte();
		while(Str[i] != g_NULL_Terminator )
		{
			i++;
			Str[i] = UART_receiveByte();
		}
		Str[i] = '\0';
	}
}

void UART_RXC_setCallBack(void(*a_ptr)(void))
{
	/* Save the address of the Call back function in a global variable */
	g_UART_RXC_callBack_ptr = a_ptr;
}

void UART_TXC_setCallBack(void(*a_ptr)(void))
{
	/* Save the address of the Call back function in a global variable */
	g_UART_TXC_callBack_ptr = a_ptr;
}

void UART_UDRE_setCallBack(void(*a_ptr)(void))
{
	/* Save the address of the Call back function in a global variable */
	g_UART_UDRE_callBack_ptr = a_ptr;
}

