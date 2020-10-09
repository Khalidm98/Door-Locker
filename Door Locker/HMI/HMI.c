/*
 * main.c
 *
 *  Created on: Oct 6, 2020
 *      Author: Khalid
 */


#include "keypad.h"
#include "lcd.h"
#include "timers.h"
#include "uart.h"


/* UART commands */
#define GET_PASS 0x69			/* get saved password */
#define NEW_PASS 0x29			/* set new password */
#define OPEN_DOOR 0x0D			/* open door */
#define THEFT_ALERT 0x7A		/* theft alert */
#define CONTROL_READY 0xC0		/* control is ready to transmit or receive */

/* constants */
#define WRONG 0					/* wrong password */
#define CORRECT 1				/* correct password */
#define PASS_SIZE 5				/* number of password digits */
#define WRONG_ATTEMPTS 3		/* max number of wrong attempts */


/* global variable indicating the status */
/* 1: idle, 0: opening door, closing door, OR theft alert */
uint8 g_idle = 1;

/* global variable containing the number of timer ticks */
uint8 g_ticks = 0;

/* global configuration struct of TIMER1 */
TIMERS_ConfigType timer1a_config = {TIMER1A, CTC_OCR1A, F_CPU_1024, DISCONNECT_OC, 0, 60000};


void new_password(void);			/* set a new password */
bool check_password(void);			/* check for current password */
void open_door(void);				/* open door in 10 sec then close it in 10 sec */
void theft_alert(void);				/* alert for a theft attempt for 1 min */
void timer_open_door(void);			/* timer callback function when opening / closing door */
void timer_theft_alert(void);		/* timer callback function when alerting for theft */


int main() {
	uint8 choice;					/* user input */
	UART_ConfigType uart_config = {ONE_BIT, DISABLE, BIT_8};

	SREG |= (1<<7);
	LCD_init();
	UART_init(&uart_config);
	new_password();					/* set up a new password at the beginning */

	while(1) {
		if (g_idle) {
			LCD_clearScreen();
			LCD_displayString("\'*\': Open Door");
			LCD_displayStringAt(1, 0, "\'#\': Change Pass");
			choice = Keypad_getPressedKey();
			_delay_ms(500);
			
			if (choice == '*') {
				bool correct = check_password();
				if (correct)
					open_door();
				else
					theft_alert();
			}

			else if (choice == '#') {
				bool correct = check_password();
				if (correct)
					new_password();
				else
					theft_alert();
			}
		}
	}
}

void new_password(void) {
	while(1) {
		uint8 i;
		uint8 pass[5];
		bool correct = CORRECT;

		LCD_clearScreen();
		LCD_displayString("Enter New Pass:");
		LCD_moveCursorTo(1,0);
		for (i = 0; i < PASS_SIZE; i++) {
			pass[i] = Keypad_getPressedKey();
			LCD_displayCharacter('*');
			_delay_ms(500);
		}

		LCD_clearScreen();
		LCD_displayString("Confirm Pass:");
		LCD_moveCursorTo(1,0);
		for (i = 0; i < PASS_SIZE; i++) {
			uint8 digit = Keypad_getPressedKey();
			LCD_displayCharacter('*');
			if (pass[i] != digit)
				correct = WRONG;
			_delay_ms(500);
		}

		if (correct) {
			UART_sendByte(NEW_PASS);
			while (UART_receiveByte() != CONTROL_READY);
			for (i = 0; i < PASS_SIZE; ++i)
				UART_sendByte(pass[i]);
			LCD_clearScreen();
			LCD_displayStringAt(0, 2, "New password");
			LCD_displayStringAt(1, 4, "is saved");
			_delay_ms(2000);
			return;
		}

		else {
			LCD_clearScreen();
			LCD_displayStringAt(0, 3, "Passwords");
			LCD_displayStringAt(1, 2, "don\'t match");
			_delay_ms(2000);
		}
	}
}

bool check_password(void) {
	uint8 i;
	uint8 attempt;
	uint8 pass[5];
	UART_sendByte(GET_PASS);
	while (UART_receiveByte() != CONTROL_READY);
	for (i = 0; i < PASS_SIZE; i++)
		pass[i] = UART_receiveByte();

	for (attempt = 0; attempt < WRONG_ATTEMPTS; attempt++) {
		bool correct = CORRECT;
		LCD_clearScreen();
		LCD_displayString("Enter your pass:");
		LCD_moveCursorTo(1,0);
		for (i = 0; i < PASS_SIZE; i++) {
			uint8 digit = Keypad_getPressedKey();
			LCD_displayCharacter('*');
			if (pass[i] != digit)
				correct = WRONG;
			_delay_ms(500);
		}

		if (correct)
			return CORRECT;

		else if (attempt < (WRONG_ATTEMPTS - 1)) {
			LCD_clearScreen();
			LCD_displayStringAt(0, 3, "WRONG PASS");
			LCD_displayStringAt(1, 3, "TRY AGAIN!");
			_delay_ms(2000);
		}
	}
	return WRONG;
}

void open_door(void) {
	UART_sendByte(OPEN_DOOR);
	while (UART_receiveByte() != CONTROL_READY);
	timer1a_config.initial_value = 41875;
	TIMERS_setCallBack(TIMER1A, CTC_OCR1A, timer_open_door);
	TIMERS_init(&timer1a_config);

	g_idle = 0;
	LCD_clearScreen();
	LCD_displayStringAt(0, 4, "Door is");
	LCD_displayStringAt(1, 3, "opening...");
}

void theft_alert(void) {
	UART_sendByte(THEFT_ALERT);
	while (UART_receiveByte() != CONTROL_READY);
	timer1a_config.initial_value = 11250;
	TIMERS_setCallBack(TIMER1A, CTC_OCR1A, timer_theft_alert);
	TIMERS_init(&timer1a_config);

	g_idle = 0;
	LCD_clearScreen();
	LCD_displayStringAt(0, 2, "7araaaamyyyy");
}

void timer_open_door(void) {
	g_ticks++;
	if (g_ticks == 2) {
		TCNT1 = 41875;
		LCD_clearScreen();
		LCD_displayStringAt(0, 4, "Door is");
		LCD_displayStringAt(1, 3, "closing...");
	}
	else if (g_ticks == 4) {
		g_idle = 1;
		g_ticks = 0;
		TIMERS_deInit(TIMER1A);
	}
}

void timer_theft_alert(void) {
	g_ticks++;
	if (g_ticks == 8) {
		g_idle = 1;
		g_ticks = 0;
		TIMERS_deInit(TIMER1A);
	}
}
