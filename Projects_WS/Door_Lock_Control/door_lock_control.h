 /******************************************************************************
 *
 * Description: Smart Door Lock System
 * File Name:	door_lock_control.h
 * Connections: - Motor PD6:PD7
 * 				- External EEPROM PC0(SCL):PC1(SDA)
 * 				- Connect UART Lines Rx->Tx
 *
 * Created on :	Sep 30, 2020
 * Author: 		Mohsen Moawad
 *
 *******************************************************************************/

#ifndef DOOR_LOCK_CONTROL_H_
#define DOOR_LOCK_CONTROL_H_

/*******************************************************************************
 *                    	  Libraries Include                                    *
 *******************************************************************************/

#include "uart.h"
#include "external_eeprom.h"
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
 * Description: Function to Set New Password in EEPROM .
 */
void SetPassword(void);

/*
 * Description: Function to Open Door For 10 Sec then Close Door for 10 Sec .
 */
void MotorOn(void);

/*
 * Description: Function to check if the received password is equal to
 * 				the old password that saved in EEPROM .
 */
void CheckPassword(void);

/*
 * Description: Function to check if there is any saved password in EEPROM
 * 				before.
 */
void EEPROM_CheckPassword(void);

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


#endif /* DOOR_LOCK_CONTROL_H_ */
