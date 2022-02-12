 /******************************************************************************
 * Module: 		Timer Driver
 * File Name: 	timer.c
 * Description: Source file for Timer Module
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
 *				- Timer1 Maximum time in 1 overflow is 65535ms ( 65.5 Sec )
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

#include "timer.h"

/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/

static void (*g_TIMER0_callBackPtr)(void) = NULL_PTR;
static void (*g_TIMER2_callBackPtr)(void) = NULL_PTR;
static void (*g_TIMER1_callBackPtr)(void) = NULL_PTR;
static uint8 g_T0clock, g_T1clock, g_T2clock ;

/*******************************************************************************
 *                       Interrupt Service Routines                            *
 *******************************************************************************/

/*
 *  Description : Interrupt Service Routine for TIMER0 CTC Mode
 */
ISR(TIMER0_COMP_vect)
{
	if(g_TIMER0_callBackPtr != NULL_PTR)
	{
		/* Call The Call Back function in the application after the timer value = OCR0 Value*/
		(*g_TIMER0_callBackPtr)() ;
	}
}

/*
 *  Description : Interrupt Service Routine for TIMER0 OverFlow(Normal) Mode
 */
ISR(TIMER0_OVF_vect)
{
	if(g_TIMER0_callBackPtr != NULL_PTR)
	{
		/* Call The Call Back function in the application after the timer value = 1023 */
		(*g_TIMER0_callBackPtr)() ;
	}
}

/*
 *  Description : Interrupt Service Routine for TIMER2 CTC Mode
 */
ISR(TIMER2_COMP_vect)
{
	if(g_TIMER2_callBackPtr != NULL_PTR)
	{
		/* Call The Call Back function in the application after the timer value = OCR0 Value*/
		(*g_TIMER2_callBackPtr)() ;
	}
}

/*
 *  Description : Interrupt Service Routine for TIMER2 OverFlow(Normal) Mode
 */
ISR(TIMER2_OVF_vect)
{
	if(g_TIMER2_callBackPtr != NULL_PTR)
	{
		/* Call The Call Back function in the application after the timer value = 1023 */
		(*g_TIMER2_callBackPtr)() ;
	}
}

/*
 *  Description : Interrupt Service Routine for TIMER1 CTC Mode
 */
ISR(TIMER1_COMPA_vect)
{
	if(g_TIMER1_callBackPtr != NULL_PTR)
	{
		/* Call The Call Back function in the application after the timer value = OCR1A Value*/
		(*g_TIMER1_callBackPtr)() ;
	}
}

/*
 *  Description : Interrupt Service Routine for TIMER1 OverFlow(Normal) Mode
 */
ISR(TIMER1_OVF_vect)
{
	if(g_TIMER1_callBackPtr != NULL_PTR)
	{
		/* Call The Call Back function in the application after the timer value = 65,535 */
		(*g_TIMER1_callBackPtr)() ;
	}
}

/*******************************************************************************
 *                      TIMER0 Configuration                                   *
 *******************************************************************************/


/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

/*
 * Description : Function to initialize the Timer driver
 * 	1. Select Timer Mode ( Normal - Compare - Square )
 * 	2. Set the required clock.
 * 	3. Set Compare Value if The Timer in Compare Mode
 * 	4. Set OC0 Pin Mode in Square Mode
 * 	5. Enable the Timer Normal-Compare Interrupt.
 * 	6. Initialize Timer0 Registers
 */
void Timer0_Init(TIMER_ConfigType *Config_ptr)
{
	/* set a global variable for clock to use it in
	 * restart timer function */
	g_T0clock = Config_ptr->clock ;

	/* Set Timer0 In Normal Mode */
	if (Config_ptr->mode == NORMAL)
	{
		/* Set Timer initial value to 0 */
		TCNT0 = 0;

		/*  Enable Timer0 Overflow Interrupt */
		TIMSK |= (1<<TOIE0);

		/* Configure the timer control register
		 * 1. Non PWM mode	=> FOC0=1
		 * 2. Normal Mode	=> WGM01=0 & WGM00=0
		 * 3. OC0 Mode 		=> COM00 & COM01 (Disconnected)
		 * 4. clock 		=> CS00 & CS01 & CS02
		 * TCCR0 => FOC0 WGM00 COM01 COM00 WGM01 CS02 CS01 CS00
		 */
		TCCR0 = (1<<FOC0) | ((Config_ptr->clock) << CS00);
	}

	/* Set Timer0 In Compare Mode with OCR0 Value */
	else if (Config_ptr->mode == COMP)
	{
		/* Set Timer initial value to 0 */
		TCNT0 = 0 ;

		/* Compare Value */
		OCR0 = (uint8)Config_ptr->OCRValue ;

		/* Interrupt Enable/Disable */
		TIMSK |= (1 << OCIE0 ) ;

		/* Configure the timer control register
		 * 1. Non PWM mode	=> FOC0=1
		 * 2. CTC Mode		=> WGM01=1 & WGM00=0
		 * 3. OC0 Mode 		=> COM00 & COM01 (DISCONNECTED)
		 * 4. clock 		=> CS00 & CS01 & CS02
		 * TCCR0 => FOC0 WGM00 COM01 COM00 WGM01 CS02 CS01 CS00
		 */
		TCCR0  = (1<<FOC0) | (1<<WGM01) | ((Config_ptr->clock) << CS00) ;
	}

	/* Set Timer0 In CTC Square Wave Mode */
	else if (Config_ptr->mode == CTC)
	{
		/* Configure PB3/OC0 Pin as output pin */
		SET_BIT(DDRB, PB3);

		/* Initial Value of The Timer TCNT*/
		TCNT0 = 0 ;

		/* Compare Value */
		OCR0 = (uint8)Config_ptr->OCRValue ;

		/* If OC0 Disconnected Mode in CTC !
		 * Make it Toggle
		 * if Disconnected ==> COMP Mode Not CTC Mode  */
		if(Config_ptr->OC == DISCONNECTED)
			Config_ptr->OC = TOGGLE ;

		/* Configure the timer control register
		 * 1. Non PWM mode	=> FOC0=1
		 * 2. CTC Mode		=> WGM01=1 & WGM00=0
		 * 3. OC0 Mode 		=> COM00 & COM01
		 * 4. clock 		=> CS00 & CS01 & CS02
		 * TCCR0 => FOC0 WGM00 COM01 COM00 WGM01 CS02 CS01 CS00
		 */

		TCCR0  = (1<<FOC0) | (1<<WGM01) | ((Config_ptr->OC) << COM00)
			   | ((Config_ptr->clock) << CS00) ;
	}

	/* Set Timer0 In PWM Mode with Duty Cycle */
	else if (Config_ptr->mode == PWM)
	{
		/* Configure PB3/OC0 Pin as output pin */
		SET_BIT(DDRB, PB3);

		/* Initial Value of The Timer TCNT*/
		TCNT0 = 0 ;

		/* Duty Cycle Value */
		OCR0 = (uint8)Config_ptr->OCRValue ;

		/* If OC0 Disconnected Mode in PWM !
		 * Make it non inverted mode
		 */
		if(Config_ptr->OC == DISCONNECTED)
			Config_ptr->OC = NON_INVERTING ;

		/* Configure the timer control register
		 * 1. PWM mode		=> FOC0=0
		 * 2. Fast PWM Mode	=> WGM01=1 & WGM00=1
		 * 3. OC0 Mode 		=> COM00 & COM01
		 * 4. clock 		=> CS00 & CS01 & CS02
		 * TCCR0 => FOC0 WGM00 COM01 COM00 WGM01 CS02 CS01 CS00
		 */

		TCCR0  = (1<<WGM00) | (1<<WGM01) | ((Config_ptr->OC) << COM00)
			   | ((Config_ptr->clock) << CS00) ;
	}
}

/*
 * Description: Function to clear the Timer0 Value to start count from ZERO
 */
void Timer0_resetTimer(void)
{
	TCNT0 = 0;
}

/*
 * Description: Function to Stop the Timer0
 * CLEAR CS02 CS01 CS00
 * TCCR0 => FOC0 WGM00 COM01 COM00 WGM01 CS02 CS01 CS00
 */
void Timer0_stopTimer(void)
{
	/* Clear Clock Bits */
	TCCR0 &= 0xF8 ;
}

/*
 * Description: Function to Restart the Timer0
 * Set CS02 CS01 CS00
 * TCCR0 => FOC0 WGM00 COM01 COM00 WGM01 CS02 CS01 CS00
 */
void Timer0_restartTimer(void)
{
	/* Clear Clock Bits */
	TCCR0 &= 0xF8 ;

	/* Set Clock Bits */
	TCCR0 |= ( g_T0clock << CS00 ) ;
}

/*
 * Description: Function to Change Ticks (Compare Value) of Timer
 * 				using also to Change Duty Cycle in PWM Mode
 */
void Timer0_Ticks(const uint8 Ticks)
{
	OCR0 = Ticks;
}

/*
 * Description: Function to set the Call Back function address for TIMER0 .
 */
void Timer0_setCallBack(void(*a_ptr)(void))
{
	/* Save the address of the Call back function in a global variable */
	g_TIMER0_callBackPtr = a_ptr;
}


/*******************************************************************************
 *                      TIMER2 Configuration                                   *
 *******************************************************************************/


/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

/*
 * Description : Function to initialize the Timer driver
 * 	1. Select Timer Mode ( Normal - Compare - Square )
 * 	2. Set the required clock.
 * 	3. Set Compare Value if The Timer in Compare Mode
 * 	4. Set OC2 Pin Mode in Square Mode
 * 	5. Enable the Timer Normal-Compare Interrupt.
 * 	6. Initialize Timer2 Registers
 */
void Timer2_Init(TIMER_ConfigType *Config_ptr)
{
	/* set a global variable for clock to use it in
	 * restart timer function */
	g_T2clock = Config_ptr->clock ;

	/* Set Timer2 In Normal Mode */
	if (Config_ptr->mode == NORMAL)
	{
		/* Set Timer initial value to 0 */
		TCNT2 = 0;

		/*  Enable Timer2 Overflow Interrupt */
		TIMSK |= (1<<TOIE2);

		/* Configure the timer control register
		 * 1. Non PWM mode	=> FOC2=1
		 * 2. Normal Mode	=> WGM21=0 & WGM20=0
		 * 3. OC2 Mode 		=> COM21 & COM20 (Disconnected)
		 * 4. clock 		=> CS22 & CS21 & CS20
		 * TCCR2 => FOC2 WGM20 COM21 COM20 WGM21 CS22 CS21 CS20
		 */
		/* AdjustTimer2Clock(Config_ptr->clock) to adjust clock values */
		TCCR2 = (1<<FOC2) | ((AdjustTimer2Clock(Config_ptr->clock)) << CS20);
	}

	/* Set Timer0 In Compare Mode with OCR0 Value */
	else if (Config_ptr->mode == COMP)
	{
		/* Set Timer initial value to 0 */
		TCNT2 = 0 ;

		/* Compare Value */
		OCR2 = (uint8)Config_ptr->OCRValue ;

		/* Interrupt Enable/Disable */
		TIMSK |= (1 << OCIE2 ) ;

		/* Configure the timer control register
		 * 1. Non PWM mode	=> FOC2=1
		 * 2. CTC Mode		=> WGM21=1 & WGM20=0
		 * 3. OC2 Mode 		=> COM20 & COM21 (DISCONNECTED)
		 * 4. clock 		=> CS20 & CS21 & CS22
		 * TCCR2 => FOC2 WGM20 COM21 COM20 WGM21 CS22 CS21 CS20
		 */
		/* AdjustTimer2Clock(Config_ptr->clock) to adjust clock values */
		TCCR2  = (1<<FOC2) | (1<<WGM21) | ((AdjustTimer2Clock(Config_ptr->clock)) << CS20) ;
	}

	/* Set Timer2 In CTC Square Wave Mode */
	else if (Config_ptr->mode == CTC)
	{
		/* Configure PD7/OC2 Pin as output pin */
		SET_BIT(DDRD, PD7);

		/* Initial Value of The Timer TCNT*/
		TCNT2 = 0 ;

		/* Compare Value */
		OCR2 = (uint8)Config_ptr->OCRValue ;

		/* If OC2 Disconnected Mode in CTC !
		 * Make it Toggle
		 * if Disconnected ==> COMP Mode Not CTC Mode  */
		if(Config_ptr->OC == DISCONNECTED)
			Config_ptr->OC = TOGGLE ;

		/* Configure the timer control register
		 * 1. Non PWM mode	=> FOC2=1
		 * 2. CTC Mode		=> WGM21=1 & WGM20=0
		 * 3. OC2 Mode 		=> COM20 & COM21
		 * 4. clock 		=> CS20 & CS21 & CS22
		 * TCCR2 => FOC2 WGM20 COM21 COM20 WGM21 CS22 CS21 CS20
		 */
		/* AdjustTimer2Clock(Config_ptr->clock) to adjust clock values */
		TCCR2  = (1<<FOC2) | (1<<WGM21) | ((Config_ptr->OC) << COM20)
			   | ((AdjustTimer2Clock(Config_ptr->clock)) << CS20) ;
	}

	/* Set Timer2 In PWM Mode with Duty Cycle */
	else if (Config_ptr->mode == PWM)
	{
		/* Configure PD7/OC2 Pin as output pin */
		SET_BIT(DDRD, PD7);

		/* Initial Value of The Timer TCNT*/
		TCNT2 = 0 ;

		/* Duty Cycle Value */
		OCR2 = (uint8)Config_ptr->OCRValue ;

		/* If OC2 Disconnected Mode in PWM !
		 * Make it non inverted mode
		 */
		if(Config_ptr->OC == DISCONNECTED)
			Config_ptr->OC = NON_INVERTING ;

		/* Configure the timer control register
		 * 1. PWM mode		=> FOC2=0
		 * 2. Fast PWM Mode	=> WGM21=1 & WGM20=1
		 * 3. OC0 Mode 		=> COM20 & COM21
		 * 4. clock 		=> CS20 & CS21 & CS22
		 * TCCR2 => FOC2 WGM20 COM21 COM20 WGM21 CS22 CS21 CS20
		 */
		/* AdjustTimer2Clock(Config_ptr->clock) to adjust clock values */
		TCCR2  = (1<<WGM20) | (1<<WGM21) | ((Config_ptr->OC) << COM20)
			   | ((AdjustTimer2Clock(Config_ptr->clock)) << CS20) ;
	}
}

/*
 * Description: Function to Adjust Timer2 Clock selected
 * From
 * 		F_CPU_CLOCK, F_CPU_8, F_CPU_64, F_CPU_256, F_CPU_1024, F_CPU_32, F_CPU_128
 * To
 * 		F_CPU_CLOCK, F_CPU_8, F_CPU_32, F_CPU_64, F_CPU_128, F_CPU_256, F_CPU_1024
 */
uint8 AdjustTimer2Clock(TIMER_Clock clk)
{
	switch (clk)
	{
		case NO_CLOCK:
			return 0;
		case F_CPU_CLOCK :
			return 1;
		case F_CPU_8 :
			return 2;
		case F_CPU_32 :
			return 3;
		case F_CPU_64 :
			return 4;
		case F_CPU_128 :
			return 5;
		case F_CPU_256 :
			return 6;
		case F_CPU_1024 :
			return 7;
		default:
			return clk;
	}
}

/*
 * Description: Function to clear the Timer2 Value to start count from ZERO
 */
void Timer2_resetTimer(void)
{
	TCNT2 = 0;
}

/*
 * Description: Function to Stop the Timer2
 * CLEAR CS22 CS21 CS20
 * TCCR2 => FOC2 WGM20 COM21 COM20 WGM21 CS22 CS21 CS20
 */
void Timer2_stopTimer(void)
{
	/* Clear Clock Bits */
	TCCR2 &= 0xF8 ;
}

/*
 * Description: Function to Restart the Timer2
 * Set CS22 CS21 CS20
 * TCCR2 => FOC2 WGM20 COM21 COM20 WGM21 CS22 CS21 CS20
 */
void Timer2_restartTimer(void)
{
	/* Clear Clock Bits */
	TCCR2 &= 0xF8 ;

	/* Set Clock Bits */
	TCCR2 |= ( (AdjustTimer2Clock(g_T2clock)) << CS00 ) ;
}

/*
 * Description: Function to Change Ticks (Compare Value) of Timer
 * 				using also to Change Duty Cycle in PWM Mode
 */
void Timer2_Ticks(const uint8 Ticks)
{
	OCR2 = Ticks;
}

/*
 * Description: Function to set the Call Back function address for TIMER2 .
 */
void Timer2_setCallBack(void(*a_ptr)(void))
{
	/* Save the address of the Call back function in a global variable */
	g_TIMER2_callBackPtr = a_ptr;
}


/*******************************************************************************
 *                      TIMER1 Configuration                                   *
 *******************************************************************************/


/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

/*
 * Description : Function to initialize the Timer driver
 * 	1. Select Timer Mode ( Normal - Compare - Square - PWM )
 * 	2. Set the required clock.
 * 	3. Set Compare Value if The Timer in Compare Mode
 * 	4. Set OC1A/OC1B Pin Mode in Square Mode
 * 	5. Enable the Timer Normal-Compare Interrupt.
 * 	6. Initialize Timer1 Registers
 */
void Timer1_Init(TIMER_ConfigType *Config_ptr)
{
	/* set a global variable for clock to use it in
	 * restart timer function */
	g_T1clock = Config_ptr->clock ;

	/* Set Timer1 In Normal Mode */
	if (Config_ptr->mode == NORMAL)
	{
		/* Set Timer initial value to 0 */
		TCNT1 = 0;

		/*  Enable Timer1 Overflow Interrupt */
		TIMSK |= (1<<TOIE1);

		/* Configure the timer control register
		 * 1. Non PWM mode	=> FOC1A=1 & FOC1B=1
		 * 2. Normal Mode	=> WGM11=0 & WGM10=0 & WGM13=0 & WGM12=0
		 * 3. OC1A/B Mode 	=> COM1A1 & COM1A0 & COM1B1 & COM1B0 (Disconnected)
		 * 4. clock 		=> CS12 & CS11 & CS10
		 * TCCR1A => COM1A1 COM1A0 COM1B1 COM1B0 FOC1A FOC1B WGM11 WGM10
		 * TCCR1B => ICNC1 ICES1 – WGM13 WGM12 CS12 CS11 CS10
		 */

		TCCR1A = (1<<FOC1A) | (1<<FOC1B) ;
		TCCR1B = ((Config_ptr->clock) << CS10) ;
	}

	/* Set Timer1 In Compare Mode with OCR1A Value */
	else if (Config_ptr->mode == COMP)
	{
		/* Set Timer initial value to 0 */
		TCNT1 = 0 ;

		/* Compare Value */
		OCR1A = Config_ptr->OCRValue ;

		/* Enable Timer1 Compare Mode Interrupt */
		TIMSK |= (1 << OCIE1A ) ;

		/* Configure the timer control register
		 * 1. Non PWM mode	=> FOC1A=1 & FOC1B=1
		 * 2. Compare Mode	=> WGM11=0 & WGM10=0 & WGM13=0 & WGM12=1
		 * 3. OC1A/B Mode	=> COM1A1 & COM1A0 & COM1B1 & COM1B0 (Disconnected)
		 * 4. clock 		=> CS12 & CS11 & CS10
		 * TCCR1A => COM1A1 COM1A0 COM1B1 COM1B0 FOC1A FOC1B WGM11 WGM10
		 * TCCR1B => ICNC1 ICES1 – WGM13 WGM12 CS12 CS11 CS10
		 */

		TCCR1A = (1<<FOC1A) | (1<<FOC1B) ;
		TCCR1B = (1<<WGM12) | ((Config_ptr->clock) << CS10) ;
	}

	/* Set Timer1 In CTC Square Wave Mode */
	else if (Config_ptr->mode == CTC)
	{
		/* Configure PD5/OC1A Pin as output pin */
		SET_BIT(DDRD, PD5);
		/* Configure PD4/OC1B Pin as output pin */
		SET_BIT(DDRD, PD4);

		/* Initial Value of The Timer TCNT*/
		TCNT1 = 0 ;

		/* Compare Value for Channel A */
		OCR1A = Config_ptr->OCRValue ;

		/* Compare Value for Channel B */
		OCR1B = Config_ptr->OCR1BValue ;

		/* If OC1A/B Disconnected Mode in CTC !
		 * Make it Toggle
		 * if Disconnected ==> COMP Mode Not CTC Mode  */
		if(Config_ptr->OC == DISCONNECTED)
			Config_ptr->OC = TOGGLE ;
		if(Config_ptr->OC1B == DISCONNECTED)
			Config_ptr->OC1B = TOGGLE ;

		/* Configure the timer control register
		 * 1. Non PWM mode	=> FOC1A=1 & FOC1B=1
		 * 2. CTC Mode		=> WGM11=0 & WGM10=0 & WGM13=0 & WGM12=1
		 * 3. OC1A/B Mode	=> COM1A1 & COM1A0 & COM1B1 & COM1B0
		 * 4. clock 		=> CS12 & CS11 & CS10
		 * TCCR1A => COM1A1 COM1A0 COM1B1 COM1B0 FOC1A FOC1B WGM11 WGM10
		 * TCCR1B => ICNC1 ICES1 – WGM13 WGM12 CS12 CS11 CS10
		 */
		TCCR1A = (1<<FOC1A) | (1<<FOC1B) | ((Config_ptr->OC) << COM1A0)
			   | ((Config_ptr->OC1B) << COM1B0);
		TCCR1B = (1<<WGM12) | ((Config_ptr->clock) << CS10) ;
	}

	/* Set Timer1 In PWM Mode with Duty Cycle */
	else if (Config_ptr->mode == PWM)
	{
		/* Configure PD5/OC1A Pin as output pin */
		SET_BIT(DDRD, PD5);

		/* Initial Value of The Timer TCNT*/
		TCNT1 = 0 ;

		/* Duty Cycle Value for Channel A */
		OCR1A = Config_ptr->OCRValue ;

		/* using ICR1 as Top Value for Duty Cycle for Channel A
		 * Duty Cycle for PWM Mode (Mode14) = OCR1A/ICR1 */
		ICR1 = Config_ptr->OCR1BValue ;

		/* If OC1A/B Disconnected Mode in CTC !
		 * Make it Toggle
		 * if Disconnected ==> COMP Mode Not CTC Mode  */
		if(Config_ptr->OC == DISCONNECTED)
			Config_ptr->OC = NON_INVERTING ;

		/* Configure the timer control register
		 * 1. PWM mode		=> FOC1A=0 & FOC1B=0
		 * 2. PWM Mode		=> WGM11=1 & WGM10=0 & WGM13=1 & WGM12=1
		 * 3. OC1A/B Mode	=> COM1A1 & COM1A0 & COM1B1 & COM1B0
		 * 4. clock 		=> CS12 & CS11 & CS10
		 * TCCR1A => COM1A1 COM1A0 COM1B1 COM1B0 FOC1A FOC1B WGM11 WGM10
		 * TCCR1B => ICNC1 ICES1 – WGM13 WGM12 CS12 CS11 CS10
		 */
		TCCR1A = ((Config_ptr->OC) << COM1A0) | (1<<WGM11)  ;
		TCCR1B = (1<<WGM13) | (1<<WGM12) | ((Config_ptr->clock) << CS10) ;
	}
}

/*
 * Description: Function to clear the Timer1 Value to start count from ZERO
 */
void Timer1_resetTimer(void)
{
	TCNT1 = 0;
}

/*
 * Description: Function to Stop the Timer1
 * CLEAR CS12 CS11 CS10
 * TCCR1B => ICNC1 ICES1 – WGM13 WGM12 CS12 CS11 CS10
 */
void Timer1_stopTimer(void)
{
	/* Clear Clock Bits */
	TCCR1B &= 0xF8 ;
}

/*
 * Description: Function to Restart the Timer1
 * CLEAR CS12 CS11 CS10
 * TCCR1B => ICNC1 ICES1 – WGM13 WGM12 CS12 CS11 CS10
 */
void Timer1_restartTimer(void)
{
	/* Clear Clock Bits */
	TCCR1B &= 0xF8 ;

	/* Set Clock Bits */
	TCCR1B |= ( g_T1clock << CS10) ;
}

/*
 * Description: Function to Change Ticks (Compare Value) of Timer
 * 				using also to Change Duty Cycle in PWM Mode
 */
void Timer1_Ticks(const uint16 Ticks1A, const uint16 Ticks1B)
{
	OCR1A = Ticks1A;
	OCR1B = Ticks1B;
}

/*
 * Description: Function to set the Call Back function address for TIMER2 .
 */
void Timer1_setCallBack(void(*a_ptr)(void))
{
	/* Save the address of the Call back function in a global variable */
	g_TIMER1_callBackPtr = a_ptr;
}
