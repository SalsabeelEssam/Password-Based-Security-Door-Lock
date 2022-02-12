 /******************************************************************************
 * Module: 		Timer Driver
 * File Name: 	timer.h
 * Description: Header file for Timer Module
 * Author: 		Mohsen Moawad
 *
 *******************************************************************************/

/*******************************************************************************
 *                          NOTES About Timer Driver                           *
 *******************************************************************************/
/*
 * Notes:		- Timer0/Timer2 Maximum time in 1 overflow is 256ms
 * 				  as 1 clock = 1ms
 * 				  ( Maximum => Using small clock = 1Mhz , and large prescaler = 1024 )
 *
 *				- Timer1 Maximum time in 1 overflow is 65535ms ( 18hr 12m 15s )
 *				  as 1 clock = 1ms
 *				  ( Maximum => Using small clock = 1Mhz , and large prescaler = 1024 )
 *
 * Pins:		- PB3/OC0  -> Square Wave and PWM Mode in Timer0
 * 				- PD7/OC2  -> Square Wave and PWM Mode in Timer2
 * 				- PD5/OC1A -> Square Wave and PWM Mode in Timer1A
 * 				- PD4/OC1B -> Square Wave  Mode in Timer1B
 *
 * Clock And Prescaler :
 * 				Time = Prescaler / F_CPU * Ticks
 * 				For clock=1Mhz and prescale F_CPU/1024 every count will take 1ms
 * 				Timer Clock => 1,000,000 / 1024 = 1Khz
 * 				clock timer period = 1 / 1Khz = 1ms
 * 				so put initial timer counter = 0  0 --> 255 (256ms per overflow)
 * 				so we need timer to overflow 4 times to get a 1 second
 *
 * Example for Config Struct:
 * 				// F_CPU = 8Mhz		Timer1 CTC Mode 	1 Sec.
 * 				TIMER_ConfigType Timer1_Config =
 * 					{.clock = F_CPU_1024, .mode = CTC, .OCRValue = 8000 };
 * 				Timer1_Init(&Timer1_Config);
 *******************************************************************************/

#ifndef TIMER_H_
#define TIMER_H_

#include "std_types.h"
#include "common_macros.h"
#include "micro_config.h"

/*******************************************************************************
 *                         Types Declaration                                   *
 *******************************************************************************/

typedef enum{
	NORMAL, COMP, CTC, PWM
}TIMER_Mode;

typedef enum{
	DISCONNECTED = 0, TOGGLE = 1, CLEAR = 2, SET = 3 , NON_INVERTING = 2 , INVERTING = 3
}OC_Pin_Mode;

typedef enum{
	NO_CLOCK, F_CPU_CLOCK, F_CPU_8, F_CPU_64, F_CPU_256, F_CPU_1024
	/* additional Clock For TIMER2 */
	, F_CPU_32, F_CPU_128
}TIMER_Clock;


typedef struct{
	/* For TIMER0:
	 * NO_CLOCK, F_CPU_CLOCK, F_CPU_8, F_CPU_64, F_CPU_256, F_CPU_1024
	 * For TIMER2:
	 * NO_CLOCK, F_CPU_CLOCK, F_CPU_8, F_CPU_32 , F_CPU_64, F_CPU_128 , F_CPU_256, F_CPU_1024
	 */
	TIMER_Clock clock ;

	/* NORMAL, COMP, CTC, PWM */
	TIMER_Mode mode ;

	/* values 0:255 ***** But in OCR1A 0:65535
	 * using also for set Duty Cycle in PWM Mode
	 * using as OCR0 , OCR1A , OCR2 Value */
	uint16 OCRValue ;

	/* CTC Mode => TOGGLE, CLEAR, SET
	 * PWM Mode => NON_INVERTING, INVERTING
	 * using as OC0 , OC1A , OC2 */
	OC_Pin_Mode OC ;

	/* values 0:65535
	 * using for OCR1B value in TIMER1
	 * and for ICR1 value as TOP value in TIMER1 PWM Duty Cycle */
	uint16 OCR1BValue ;

	/* CTC Mode => TOGGLE, CLEAR, SET
	 * PWM Mode => NON_INVERTING, INVERTING */
	OC_Pin_Mode OC1B ;
}TIMER_ConfigType;


/*******************************************************************************
 *                     TIMER0 Functions Prototypes                             *
 *******************************************************************************/

/*
 * Description : Function to initialize the Timer driver
 * 	1. Select Timer Mode ( Normal - Compare - Square - PWM )
 * 	2. Set the required clock.
 * 	3. Set Compare Value if The Timer in Compare Mode
 * 	4. Set OC0 Pin Mode in Square Mode
 * 	5. Enable the Timer Normal-Compare Interrupt.
 * 	6. Initialize Timer0 Registers
 */
void Timer0_Init(TIMER_ConfigType *Config_ptr);

/*
 * Description: Function to clear the Timer0 Value to start count from ZERO
 */
void Timer0_resetTimer(void);

/*
 * Description: Function to Stop the Timer0
 * CLEAR CS02 CS01 CS00
 * TCCR0 => FOC0 WGM00 COM01 COM00 WGM01 CS02 CS01 CS00
 */
void Timer0_stopTimer(void);

/*
 * Description: Function to Restart the Timer0
 * Set CS02 CS01 CS00
 * TCCR0 => FOC0 WGM00 COM01 COM00 WGM01 CS02 CS01 CS00
 */
void Timer0_restartTimer(void);

/*
 * Description: Function to Change Ticks (Compare Value) of Timer
 * 				using also to Change Duty Cycle in PWM Mode
 */
void Timer0_Ticks(const uint8 Ticks);

/*
 * Description: Function to set the Call Back function address For TIMER0.
 */
void Timer0_setCallBack(void(*a_ptr)(void));


/*******************************************************************************
 *                     TIMER2 Functions Prototypes                             *
 *******************************************************************************/

/*
 * Description : Function to initialize the Timer driver
 * 	1. Select Timer Mode ( Normal - Compare - Square - PWM )
 * 	2. Set the required clock.
 * 	3. Set Compare Value if The Timer in Compare Mode
 * 	4. Set OC2 Pin Mode in Square Mode
 * 	5. Enable the Timer Normal-Compare Interrupt.
 * 	6. Initialize Timer2 Registers
 */
void Timer2_Init(TIMER_ConfigType *Config_ptr);

/*
 * Description: Function to Adjust Timer2 Clock selected
 * From
 * 		F_CPU_CLOCK, F_CPU_8, F_CPU_64, F_CPU_256, F_CPU_1024, F_CPU_32, F_CPU_128
 * To
 * 		F_CPU_CLOCK, F_CPU_8, F_CPU_32, F_CPU_64, F_CPU_128, F_CPU_256, F_CPU_1024
 */
uint8 AdjustTimer2Clock(TIMER_Clock clk);

/*
 * Description: Function to clear the Timer2 Value to start count from ZERO
 */
void Timer2_resetTimer(void);

/*
 * Description: Function to Stop the Timer2
 * CLEAR CS22 CS21 CS20
 * TCCR2 => FOC2 WGM20 COM21 COM20 WGM21 CS22 CS21 CS20
 */
void Timer2_stopTimer(void);

/*
 * Description: Function to Restart the Timer2
 * Set CS22 CS21 CS20
 * TCCR2 => FOC2 WGM20 COM21 COM20 WGM21 CS22 CS21 CS20
 */
void Timer2_restartTimer(void);

/*
 * Description: Function to Change Ticks (Compare Value) of Timer
 * 				using also to Change Duty Cycle in PWM Mode
 */
void Timer2_Ticks(const uint8 Ticks);

/*
 * Description: Function to set the Call Back function address for TIMER2 .
 */
void Timer2_setCallBack(void(*a_ptr)(void));

/*******************************************************************************
 *                     TIMER1 Functions Prototypes                             *
 *******************************************************************************/

/*
 * Description : Function to initialize the Timer driver
 * 	1. Select Timer Mode ( Normal - Compare - Square - PWM )
 * 	2. Set the required clock.
 * 	3. Set Compare Value if The Timer in Compare Mode
 * 	4. Set OC1A/B Pin Mode in Square Mode
 * 	5. Enable the Timer Normal-Compare Interrupt.
 * 	6. Initialize Timer1 Registers
 */
void Timer1_Init(TIMER_ConfigType *Config_ptr);

/*
 * Description: Function to clear the Timer1 Value to start count from ZERO
 */
void Timer1_resetTimer(void);

/*
 * Description: Function to Stop the Timer1
 * CLEAR CS12 CS11 CS10
 * TCCR1B => ICNC1 ICES1 – WGM13 WGM12 CS12 CS11 CS10
 */
void Timer1_stopTimer(void);

/*
 * Description: Function to Restart the Timer1
 * Set CS12 CS11 CS10
 * TCCR1B => ICNC1 ICES1 – WGM13 WGM12 CS12 CS11 CS10
 */
void Timer1_restartTimer(void);

/*
 * Description: Function to Change Ticks (Compare Value) of Timer
 * 				using also to Change Duty Cycle in PWM Mode
 */
void Timer1_Ticks(const uint16 Ticks1A, const uint16 Ticks1B);

/*
 * Description: Function to set the Call Back function address For TIMER1.
 */
void Timer1_setCallBack(void(*a_ptr)(void));

#endif /* TIMER_H_ */
