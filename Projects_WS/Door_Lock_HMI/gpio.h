 /******************************************************************************
 * Module: 		GPIO Driver
 * File Name: 	gpio.h
 * Description: Header file for GPIO Module
 * Author: 		Mohsen Moawad
 *
 *******************************************************************************/

/*******************************************************************************
 *                          NOTES About GPIO Driver                            *
 *******************************************************************************/
/*
 * Notes:		- 	DDRA    _SFR_IO8(0x1A)		ASCII of A = 0x41
 *					DDRB    _SFR_IO8(0x17)		ASCII of B = 0x42
 *					DDRC    _SFR_IO8(0x14)		ASCII of C = 0x43
 *					DDRD    _SFR_IO8(0x11)		ASCII of D = 0x44
 *					General Equation : ((D - pin)*3) + 0x11
 *					Example pin = A
 *						(D-A)= 3 => 3*3 = 9 => 9 + 0x11 = 0x1A
 *
 *				- 	PINA    _SFR_IO8(0x19)
 *					PINB    _SFR_IO8(0x16)
 *					PINC    _SFR_IO8(0x13)
 *					PIND    _SFR_IO8(0x10)
 *					General Equation : ((D - pin)*3) + 0x10
 *
 *				 - 	PORTA    _SFR_IO8(0x1B)
 *					PORTB    _SFR_IO8(0x18)
 *					PORTC    _SFR_IO8(0x15)
 *					PORTD    _SFR_IO8(0x12)
 *					General Equation : ((D - pin)*3) + 0x12
 *
 * Example :	- Set PA1 Output
 * 						pinMode(A,PA1,OUTPUT)			or pinMode(A,1,1)
 * 				- Set PB2 INPUT
 * 						pinMode(B,PB2,INPUT)			or pinMode(B,2,0)
 * 				- Set PA1 HIGH
 * 						pinWrite(A,PA1,HIGH)			or pinWrite(A,1,1)
 * 				- Read PB2 state
 * 						PB2.state = pinRead(B,PB2)		or pinRead(B,2)
 *
 * *******************************************************************************/

#ifndef GPIO_H_
#define GPIO_H_

	#ifndef	SET_BIT
		/* Set a certain bit in any register */
		#define SET_BIT(REG,BIT) (REG|=(1<<BIT))
	#endif

	#ifndef CLEAR_BIT
		/* Clear a certain bit in any register */
		#define CLEAR_BIT(REG,BIT) (REG&=(~(1<<BIT)))
	#endif

	#ifndef BIT_IS_SET
		/* Check if a specific bit is set in any register and return true if yes */
		#define BIT_IS_SET(REG,BIT) ( REG & (1<<BIT) )
	#endif

	#ifndef OUTPUT
		#define OUTPUT 	(1u)
		#define INPUT 	(0u)
	#endif

	#ifndef HIGH
		#define HIGH	(1u)
		#define LOW		(0u)
	#endif

	#define A		(0x41)		/* ASCII value of A */
	#define B		(0x42)		/* ASCII value of B */
	#define C		(0x43)		/* ASCII value of C */
	#define D		(0x44)		/* ASCII value of D */
	#define __DDR(ABCD)		(_SFR_IO8(((D - (ABCD))*3) + 0x11))
	#define __PORT(ABCD)		(_SFR_IO8(((D - (ABCD))*3) + 0x12))
	#define __PIN(ABCD)		(_SFR_IO8(((D - (ABCD))*3) + 0x10))
	/*
	 * Description: Macro to set pin Direction
	 * 				Initial Value , pin LOW
	 * Example:		pinMode(A, PA0, OUTPUT);
	 */
	#define pinMode(ABCD,pin,mode) (((mode) == (OUTPUT)) ? \
			(SET_BIT(__DDR(ABCD),(pin))),\
			(pinWrite(ABCD,pin,LOW))\
			: (CLEAR_BIT(__DDR(ABCD),(pin))))

	/*
	 * Description: Macro to set pin Output value
	 * Example:		pinWrite(A, PA0, HIGH);
	 */
	#define pinWrite(ABCD,pin,value) (((value) == (HIGH)) ? \
			(SET_BIT(__PORT(ABCD),(pin))) \
			: (CLEAR_BIT(__PORT(ABCD),(pin))))

	/*
	 * Description: Macro to read pin input state
	 * Example:		state = pinRead(A, PA0);
	 */
	#define pinRead(ABCD,pin) \
	((BIT_IS_SET(__PIN(ABCD),(pin))) ? (HIGH) : (LOW))


#endif /* GPIO_H_ */
