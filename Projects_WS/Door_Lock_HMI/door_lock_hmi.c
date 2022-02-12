 /******************************************************************************
 *
 * Description: Smart Door Lock System
 * File Name:	door_lock_hmi.c
 * Connections: - LCD Data on PORTB : PD0:PD7 -> PB0:PB7
 * 				- LCD RS/RW/E -> PD5:PD7
 * 				- Keypad Rows A:D -> PA0:PA3
 * 				- Keypad Col 1:4 -> PA4:PA7
 * 				- Connect UART Lines Rx->Tx
 * 				- Connect Buzzer PC0 with Transistor circuit
 *
 * Created on :	Sep 30, 2020
 * Author: 		Mohsen Moawad
 *
 *******************************************************************************/

#include "door_lock_hmi.h"
#define F_CPU 8000000UL


/*******************************************************************************
 *                     	   Global Variables                                    *
 *******************************************************************************/

/* Global Variable to store the password entered from keypad */
uint8 g_password[PASS_SIZE];

/* Global variable to store the re-entered password from keypad */
uint8 g_rePassword[PASS_SIZE];

/* Global Counter For Password Array */
uint8 count = 0 ;

/* Root Password , to reset the password */
uint8 ROOT_PASS[PASS_SIZE] = {2,6,4,9,5} ;

/* Global Delay Flag => flag is set when Timer callback function is called */
bool g_delayFlag = FALSE ;

/* Global variable contain the ticks count of Timer2 */
uint16 g_T2_tick = 0 ;

/*******************************************************************************
 *                    		   Main Function                                   *
 *******************************************************************************/

int main(void)
{
	/* Global Interrupt For Timer Interrupt */
	sei();

	/* Initialize LCD */
	LCD_init();
	LCD_clearScreen();

	/* Initialize UART */
	UART_ConfigType UART_Config = {.s_BaudRate = BR9600 , .s_NULL_Terminator = '#' };
	UART_init(&UART_Config);

	/* Initialize Timer1*/
	/* F_CPU = 8Mhz		Timer1 COMP Mode 	1 Sec. */
	 TIMER_ConfigType Timer1_Config = {.clock = F_CPU_1024, .mode = COMP, .OCRValue = 8000 };
	 Timer1_Init(&Timer1_Config);
	 Timer1_stopTimer();
	 Timer1_setCallBack(Timer1_CallBack);

	/* Initialize Timer2*/
	/* F_CPU = 8Mhz		Timer2 Normal Mode 	32 Milli/OVF.
	 * Used For Software TimeOut
	 */
	 TIMER_ConfigType Timer2_Config = {.clock = F_CPU_1024, .mode = NORMAL};
	 Timer2_Init(&Timer2_Config);
	 Timer2_stopTimer();
	 Timer2_setCallBack(Timer2_CallBack);

	UART_sendByte(READY) ;
	if(UART_receiveByte() == PASS_NOT_FOUND)
		EnterNewPass();
	LCD_clearScreen();

	while(1)
	{
		MainScreen();
	}

}



/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

/*
 * Description: Function to Display The Main Screen and get operation from User.
 */
void MainScreen(void)
{
	LCD_displayStringRowColumn(0,0,"+ : Change PASS");
	LCD_displayStringRowColumn(1,0,"- : Open Door");

	/* Press + To change Password */
	if(KeyPad_getPressedKey() == '+')
		ChangePass();

	/* Press - To Open Door */
	else if (KeyPad_getPressedKey() == '-')
		OpenDoor();


	/* Hidden Option to Access Root Password
	 * Press = for 3 sec. To Reset Password using Root Password */
	else if (KeyPad_getPressedKey() == '=')
	{
		T1_delay_sec(3);
		if (KeyPad_getPressedKey() == '=')
		{
			LCD_clearScreen();
			LCD_displayStringRowColumn(0,0,"Enter Root PASS");
			T1_delay_msec(500);
			for (count=0 ; count < PASS_SIZE ; count++)
			{
				Timer2_restartTimer();	/* For Software TimeOut , 10 Sec. */
				g_password[count] = KeyPad_getPressedKey();
				Timer2_stopTimer();		/* Stop Timer2 if 10 Sec. Doesn't Passed */
				g_T2_tick = 0 ;			/* Reset ticks counter of Timer2 */
				T1_delay_msec(500);
				LCD_displayStringRowColumn(1,count,"*");
				if (g_password[count] != ROOT_PASS[count] )
					MainScreen();
			}
			/* Reset Password if Password = Root Password */
			UART_sendByte(READY) ;
			while(UART_receiveByte() != READY);
			UART_sendByte(CHANGE_PASSWORD);
			EnterNewPass();
		}
	}
}

/*
 * Description: Function to Enter New Password and send the password to Control ECU
 * 				To Store the password in EEPROM.
 */
void EnterNewPass(void)
{
	LCD_clearScreen();
	LCD_displayStringRowColumn(0,0,"Enter New PASS");
	for (count=0 ; count < PASS_SIZE ; count++)
	{
		Timer2_restartTimer();	/* For Software TimeOut , 10 Sec. */
		g_password[count] = KeyPad_getPressedKey();
		Timer2_stopTimer();		/* Stop Timer2 if 10 Sec. Doesn't Passed */
		g_T2_tick = 0 ;			/* Reset ticks counter of Timer2 */
		T1_delay_msec(500);
		LCD_displayStringRowColumn(1,count,"*");
	}
	/* ReEnter Password to be sure from password */
	LCD_clearScreen();
	LCD_displayStringRowColumn(0,0,"ReEnter PASS");
	for (count=0 ; count < PASS_SIZE ; count++)
	{
		Timer2_restartTimer();	/* For Software TimeOut , 10 Sec. */
		g_rePassword[count] = KeyPad_getPressedKey();
		Timer2_stopTimer();		/* Stop Timer2 if 10 Sec. Doesn't Passed */
		g_T2_tick = 0 ;			/* Reset ticks counter of Timer2 */
		T1_delay_msec(500);
		LCD_displayStringRowColumn(1,count,"*");
	}

	/* check if the two password matched or not */
	for (count=0 ; count < PASS_SIZE ; count++)
	{
		if( g_password[count] != g_rePassword[count] )
		{
			/* Passwords Entered are not matched */
			LCD_clearScreen();
			LCD_displayStringRowColumn(0,0,"PASS not matched");
			T1_delay_sec(2);

			/* Go To EnterNewPass() Function Again to Enter The Password */
			EnterNewPass();
		}
	}

	/* Passwords Entered are matched */
	LCD_clearScreen();
	LCD_displayStringRowColumn(0,0,"Confirmed");

	/* Send The Password To Contol ECU To store it in EEPROM */
	for (count=0 ; count < PASS_SIZE ; count++)
	{
		UART_sendByte(READY) ;
		while(UART_receiveByte() != READY);
		UART_sendByte(g_password[count]);
	}
	T1_delay_msec(1000);
}

/*
 * Description: Function to change the old password and Set New Password .
 */
void ChangePass(void)
{
	T1_delay_msec(500);
	LCD_clearScreen();
	LCD_displayStringRowColumn(0,0,"Enter Old PASS");
	for (count=0 ; count < PASS_SIZE ; count++)
	{
		Timer2_restartTimer();	/* For Software TimeOut , 10 Sec. */
		g_password[count] = KeyPad_getPressedKey();
		Timer2_stopTimer();		/* Stop Timer2 if 10 Sec. Doesn't Passed */
		g_T2_tick = 0 ;			/* Reset ticks counter of Timer2 */
		T1_delay_msec(350);
		LCD_displayStringRowColumn(1,count,"*");
	}
	UART_sendByte(READY) ;
	while(UART_receiveByte() != READY);
	UART_sendByte(CHECK_PASSWORD);


	for (count=0 ; count < PASS_SIZE ; count++)
	{
		while(UART_receiveByte() != READY);
		UART_sendByte(g_password[count]);
	}

	/* Password Matched the Old Password */
	if(UART_receiveByte() == MATCH)
	{
		UART_sendByte(READY) ;
		while(UART_receiveByte() != READY);
		UART_sendByte(CHANGE_PASSWORD);
		EnterNewPass();
	}


	/* Password Doesn't Match the Old Password */
	else
	{
		static int i = 0 ;
		i++ ;
		if(i == 3)
		{
			i=0 ;
			BlockSystem();
		}
		ChangePass();
	}

}

/*
 * Description: Function to Block The System for 1 Min. if the password Entered
 * 				3 Times Wrong! .
 */
void BlockSystem(void)
{
	LCD_clearScreen();
	LCD_displayStringRowColumn(0,0,"System Blocked");

	/* Buzzer Start and Block System for 1 Min. */
	pinMode(C,PC0,OUTPUT);
	pinWrite(C,PC0,HIGH);
	T1_delay_sec(60);
	/* Buzzer Stop */
	pinWrite(C,PC0,LOW);
}

/*
 * Description: Function to Open Door when Password is Right .
 */
void OpenDoor(void)
{
	T1_delay_msec(500);
	LCD_clearScreen();
	LCD_displayStringRowColumn(0,0,"Enter  PASS");
	for (count=0 ; count < PASS_SIZE ; count++)
	{
		Timer2_restartTimer();	/* For Software TimeOut , 10 Sec. */
		g_password[count] = KeyPad_getPressedKey();
		Timer2_stopTimer();		/* Stop Timer2 if 10 Sec. Doesn't Passed */
		g_T2_tick = 0 ;			/* Reset ticks counter of Timer2 */

		T1_delay_msec(350);
		LCD_displayStringRowColumn(1,count,"*");
	}
	UART_sendByte(READY) ;
	while(UART_receiveByte() != READY);
	UART_sendByte(CHECK_PASSWORD);

	for (count=0 ; count < PASS_SIZE ; count++)
	{
		while(UART_receiveByte() != READY);
		UART_sendByte(g_password[count]);
	}

	/* Password Matched the Old Password */
	if(UART_receiveByte() == MATCH)
	{
		UART_sendByte(READY) ;
		while(UART_receiveByte() != READY);
		UART_sendByte(OPEN_DOOR);

		/* Display On LCD that Door Open/Close while Door is Open/Close */
		LCD_clearScreen();
		LCD_displayStringRowColumn(0,0,"Door Open");
		T1_delay_sec(10);
		LCD_clearScreen();
		LCD_displayStringRowColumn(0,0,"Door Close");
		T1_delay_sec(10);
	}

	/* Password Doesn't Match the Old Password */
	else
	{
		static int i = 0 ;
		i++ ;
		if(i == 3)
		{
			i=0 ;
			BlockSystem();
		}
		OpenDoor();
	}
}

/*
 * Description: Function to delay in msec using Timer1
 * 				 Maximun value for msec 8000
 */
void T1_delay_msec(uint16 msec)
{
	/* 8000 => Timer1 Ticks for getting 1 sec.
	 * F_CPU = 8Mhz		Prescaler = 1024	ticks = 8000 for 1 sec*/
	Timer1_Ticks((msec * 8 ),0);

	/* Reset Timer1 to Zero */
	Timer1_resetTimer();

	/* Start Timer1*/
	Timer1_restartTimer();

	/* wait until Timer1 ISR Fired and set g_delayFlag = TRUE */
	while(!g_delayFlag);

	/* clear delay flag */
	g_delayFlag = FALSE ;
}

/*
 * Description: Function To Delay in sec by calling delay_ms function with 1000 ms.
 */
void T1_delay_sec(uint16 sec)
{
	int i;
	/* recall delay milli function for N sec.*/
	for(i = 0 ; i < sec ; i++)
	{
		/* Call Delay Milli Function by 1000 milli (1sec) */
		T1_delay_msec(1000);
	}
}

/*
 * Description: Call Back Function of Timer1 ISR => Set delay flag
 */
void Timer1_CallBack(void)
{
	/* Set Delay Flag */
	g_delayFlag = TRUE ;

	/* Stop Timer1 */
	Timer1_stopTimer();

	/* Reset Timer1 to Zero */
	Timer1_resetTimer();
}

/*
 * Description: Call Back Function of Timer2
 * 				Call MainScreen() Function After 10 Sec.
 * 				Software Timeout after 10 Sec.
 */
void Timer2_CallBack(void)
{
	/* Timer2 1 OvF -> 32 milli
	 * 1 sec. = 31 OvF
	 */

	g_T2_tick++ ;

	/* if Timer2 Count 10 Sec. Reset System */
	if(g_T2_tick == 310)
	{
		g_T2_tick = 0 ;

		/* Enable WatchDog To Reset The System */
		WDTCR = (1<<WDE);

		/*
		 * Calling MainScreen() instead of using WatchDog Causing Error!
		 * is it Stack overflow?!
		 */
	}

}


