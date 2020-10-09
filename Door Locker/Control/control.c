/*
 * control.c
 *
 *  Created on: Oct 7, 2020
 *      Author: Khalid
 */


#include "external_eeprom.h"
#include "timers.h"
#include "uart.h"


/* UART commands */
#define GET_PASS 0x69			/* get saved password */
#define NEW_PASS 0x29			/* set new password */
#define OPEN_DOOR 0x0D			/* open door */
#define THEFT_ALERT 0x7A		/* theft alert */
#define CONTROL_READY 0xC0		/* control is ready to transmit or receive */

/* constants */
#define PASS_SIZE 5				/* number of password digits */
#define PASS_ADDRESS 0x00AD		/* address of password in eeprom */


/* global variable containing the number of timer ticks */
uint8 g_ticks = 0;

/* global configuration struct of TIMER1 */
TIMERS_ConfigType timer1a_config = {TIMER1A, CTC_OCR1A, F_CPU_1024, DISCONNECT_OC, 0, 60000};


void new_password(void);			/* save a new password in EEPROM */
void get_password(void);			/* get current password from EEPROM */
void open_door(void);				/* rotate motor CW for 10 sec then CCW for 10 sec */
void theft_alert(void);				/* turn on buzzer to alert for a theft attempt for 1 min */
void timer_open_door(void);			/* timer callback function when opening / closing door */
void timer_theft_alert(void);		/* timer callback function when alerting for theft */


int main() {
	uint8 command;					/* received command via UART from HMI microcontroller */
	UART_ConfigType uart_config = {ONE_BIT, DISABLE, BIT_8};
	
	SET_BIT(DDRA,PA0);				/* configure buzzer pin (PA0) as output pin */
	DDRB |= 0x03;					/* configure motor pins (PB0, PB1) as output pins */
	CLEAR_BIT(PORTA,PA0);			/* turn off buzzer initially */
	PORTB &= 0xFC;					/* turn off motor initially */
	SREG |= (1<<7);
	EEPROM_init();
	UART_init(&uart_config);

	while(1) {
		command = UART_receiveByte();
		switch(command) {
			case GET_PASS:		get_password();		break;
			case NEW_PASS:		new_password();		break;
			case OPEN_DOOR:		open_door();		break;
			case THEFT_ALERT:	theft_alert();		break;
		}
	}
}

void new_password(void) {
	uint8 i;
	uint8 pass[5];
	UART_sendByte(CONTROL_READY);
	for (i = 0; i < PASS_SIZE; i++)
		pass[i] = UART_receiveByte();
	for (i = 0; i < PASS_SIZE; i++) {
		EEPROM_writeByte(PASS_ADDRESS + i, pass[i]);
		_delay_ms(10);
	}
}

void get_password(void) {
	uint8 i;
	uint8 pass[5];
	for (i = 0; i < PASS_SIZE; i++) {
		EEPROM_readByte(PASS_ADDRESS + i, &pass[i]);
		_delay_ms(10);
	}
	UART_sendByte(CONTROL_READY);
	for (i = 0; i < PASS_SIZE; i++)
		UART_sendByte(pass[i]);
}

void open_door(void) {
	UART_sendByte(CONTROL_READY);
	timer1a_config.initial_value = 41875;
	TIMERS_setCallBack(TIMER1A, CTC_OCR1A, timer_open_door);
	TIMERS_init(&timer1a_config);
	SET_BIT(PORTB,PB0);
}

void theft_alert(void) {
	UART_sendByte(CONTROL_READY);
	timer1a_config.initial_value = 11250;
	TIMERS_setCallBack(TIMER1A, CTC_OCR1A, timer_theft_alert);
	TIMERS_init(&timer1a_config);
	SET_BIT(PORTA,PA0);
}

void timer_open_door(void) {
	g_ticks++;
	if (g_ticks == 2) {
		TCNT1 = 41875;
		PORTB ^= 0x03;
	}
	else if (g_ticks == 4) {
		CLEAR_BIT(PORTB,PB1);
		g_ticks = 0;
		TIMERS_deInit(TIMER1A);
	}
}

void timer_theft_alert(void) {
	g_ticks++;
	if (g_ticks == 8) {
		CLEAR_BIT(PORTA,PA0);
		g_ticks = 0;
		TIMERS_deInit(TIMER1A);
	}
}
