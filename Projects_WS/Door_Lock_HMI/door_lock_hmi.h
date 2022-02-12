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

#ifndef DOOR_LOCK_HMI_H_
#define DOOR_LOCK_HMI_H_

/*******************************************************************************
 *                    	  Libraries Include                                    *
 *******************************************************************************/

#include "lcd.h"
#include "keypad.h"
#include "uart.h"
#include "timer.h"
#include "gpio.h"


/*******************************************************************************
 *                   	   Macros Definition                                   *
 *******************************************************************************/

/* For UART confirmation */
#define READY				 	0x01
#define CHECK_PASSWORD		 	0x02
#define MATCH					0x03
#define DONT_MATCH				0x04
#define CHANGE_PASSWORD			0x05
#define OPEN_DOOR				0x06
#define PASS_FOUND				0x07
#define PASS_NOT_FOUND			0x08

/* Password Size */
#define PASS_SIZE 5

/* EEPROM MACROS */
#define PASS_ADDRESS 0x0100


/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description: Function to Display The Main Screen and get operation from User.
 */
void MainScreen(void);

/*
 * Description: Function to Enter New Password and send the password to Control ECU
 * 				To Store the password in EEPROM.
 */
void EnterNewPass(void);

/*
 * Description: Function to change the old password and Set New Password .
 */
void ChangePass(void);

/*
 * Description: Function to Block The System for 1 Min. if the password Entered
 * 				3 Times Wrong! .
 */
void BlockSystem(void);

/*
 * Description: Function to Open Door when Password is Right .
 */
void OpenDoor(void);

/*
 * Description: Function to delay in msec using Timer1
 * 				 Maximun value for msec 8000
 */
void T1_delay_msec(uint16 msec);

/*
 * Description: Function To Delay in sec by calling delay_ms function with 1000 ms.
 */
void T1_delay_sec(uint16 sec);

/*
 * Description: Call Back Function of Timer1 ISR => Set delay flag
 */
void Timer1_CallBack(void);

/*
 * Description: Call Back Function of Timer2
 * 				Call MainScreen() Function After 10 Sec.
 * 				Software Timeout after 10 Sec.
 */
void Timer2_CallBack(void);


#endif /* DOOR_LOCK_HMI_H_ */
