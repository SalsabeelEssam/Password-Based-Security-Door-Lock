# Password-Based-Security-Door-Lock

using Atmega32 (2 ECU'S), to transfer passwords between them we use UART to store data in
EEPROM, It takes a password from the user and allows him to enter the garage by opening the door
for one minute and then closing a door for another one minute too but if he entered a wrong password
3 times in a row the system will shut down for 5 minutes it will show a warning message and a buzzer
will be activated.
