 /******************************************************************************
 *
 * Description: Smart Door Lock System
 * File Name:	door_lock_control.c
 * Connections: - Motor PD6:PD7
 * 				- External EEPROM PC0(SCL):PC1(SDA)
 * 				- Connect UART Lines Rx->Tx
 *
 * Created on :	Sep 30, 2020
 * Author: 		Mohsen Moawad
 *
 *******************************************************************************/


#include "door_lock_control.h"


/*******************************************************************************
 *                     	   Global Variables                                    *
 *******************************************************************************/

/* Save The Password */
uint8 g_password[PASS_SIZE] = {0} ;


/* Data Received from UART */
uint8 Data;

/* Save Received Password from EEPROM */
uint8 g_EEPassword[PASS_SIZE] = {0} ;

/* Global Counter For Password Array */
uint8 count = 0 ;

/* Global Delay Flag => flag is set when Timer callback function is called */
bool g_delayFlag = FALSE ;


/*******************************************************************************
 *                    		   Main Function                                   *
 *******************************************************************************/
int main(void)
{
	/* Enable Global Interrupt For Timer */
	sei();

	/* Initialize UART */
	UART_ConfigType UART_Config = {.s_BaudRate = BR9600 , .s_NULL_Terminator = '#' };
	UART_init(&UART_Config);


	/* Initialize External EEPROM */
	EEPROM_init();

	/* Initialize Timer1*/
	/* F_CPU = 8Mhz		Timer1 COMP Mode 	1 Sec. */
	TIMER_ConfigType Timer1_Config = {.clock = F_CPU_1024, .mode = COMP, .OCRValue = 8000 };
	Timer1_Init(&Timer1_Config);
	Timer1_stopTimer();
	Timer1_setCallBack(Timer1_CallBack);

	/* Motor Initialize */
	/* Set Pins O/P and LOW */
	pinMode(D,PD6,OUTPUT);
	pinMode(D,PD7,OUTPUT);

	/* check if there is password in EEPROM */
	while (UART_receiveByte() != READY);
	EEPROM_CheckPassword();

	while(1)
	{
		while (UART_receiveByte() != READY);
		UART_sendByte(READY) ;
		Data = UART_receiveByte();
		if(Data == CHECK_PASSWORD)
			CheckPassword();
		else if (Data == CHANGE_PASSWORD)
			SetPassword();
		else if (Data == OPEN_DOOR)
			MotorOn();

	}
}

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

/*
 * Description: Function to Set New Password in EEPROM .
 */
void SetPassword(void)
{
	for (count = 0 ; count < PASS_SIZE ; count++)
	{
		/* wait until UART receive a READY byte from HMI ECU*/
		while (UART_receiveByte() != READY);

		/* UART Ready To receive Data */
		UART_sendByte(READY);

		/* UART Receive Data */
		g_password[count] = UART_receiveByte();

		/* Write Password in External EEPROM */
		EEPROM_writeByte((PASS_ADDRESS + count ) , g_password[count]);
		T1_delay_msec(10);
	}

}

/*
 * Description: Function to Open Door For 10 Sec then Close Door for 10 Sec .
 */
void MotorOn(void)
{
	/* Open Door For 10 Sec */
	pinWrite(D,PD6,HIGH);
	pinWrite(D,PD7,LOW);
	T1_delay_sec(10);

	/* Close Door For 10 Sec */
	pinWrite(D,PD6,LOW);
	pinWrite(D,PD7,HIGH);
	T1_delay_sec(10);

	/* Stop Motor at Closed Mode */
	pinWrite(D,PD6,LOW);
	pinWrite(D,PD7,LOW);
}

/*
 * Description: Function to check if the received password is equal to
 * 				the old password that saved in EEPROM .
 */
void CheckPassword(void)
{
	for (count = 0 ; count < PASS_SIZE ; count++)
	{
		/* UART Ready To receive Data */
		UART_sendByte(READY);

		/* UART Receive Data */
		g_password[count] = UART_receiveByte();

		/* Read Password from External EEPROM */
		EEPROM_readByte((PASS_ADDRESS + count ) , &g_EEPassword[count]);
		T1_delay_msec(10);

	}
	for (count = 0 ; count < PASS_SIZE ; count++)
	{
		if(g_password[count] != g_EEPassword[count])
		{
			UART_sendByte(DONT_MATCH);
			return ;
		}
	}
	UART_sendByte(MATCH);
}

/*
 * Description: Function to check if there is any saved password in EEPROM
 * 				before.
 */
void EEPROM_CheckPassword(void)
{
	for (count = 0 ; count < PASS_SIZE ; count++)
	{
		/* check if there is Password in External EEPROM */
		EEPROM_readByte((PASS_ADDRESS + count ) , &g_EEPassword[count]);
		T1_delay_msec(10);

		/* Initial Value of EEPROM = 0xFF, So if its not equal to 0xFF
		 * there is password saved there */
		if(g_EEPassword[count] != 0xFF)
		{
			UART_sendByte(PASS_FOUND);
			return;
		}
	}

	/* If Password not Found -> set New Password  */
	UART_sendByte(PASS_NOT_FOUND);

	/* Set New Password*/
	SetPassword();

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
	g_delayFlag = TRUE ;

	/* Stop Timer1 */
	Timer1_stopTimer();

	/* Reset Timer1 to Zero */
	Timer1_resetTimer();
}

