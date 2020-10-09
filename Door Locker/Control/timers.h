/* Driver for Atmega16 TIMERS modules */

#ifndef SPI_H_
#define SPI_H_


#include "std_types.h"
#include "micro_config.h"
#include "common_macros.h"


typedef enum {
	TIMER0, TIMER1, TIMER1A, TIMER1B, TIMER2
} TIMERS_Num;

typedef enum {
	NORMAL, PWM_PHASE_CORRECT, CTC, FAST_PWM,
	CTC_OCR1A = 4, CTC_ICR1 = 12,
	PHASE_8 = 1, PHASE_9 = 2, PHASE_10 = 3, PHASE_ICR1 = 10, PHASE_OCR1A = 11,
	FAST_8 = 5, FAST_9 = 6, FAST_10 = 7, FAST_ICR1 = 14, FAST_OCR1A = 15,
	FREQ_ICR1 = 8, FREQ_OCR1A = 9
} TIMERS_Mode;

typedef enum {
	NO_CLOCK, F_CPU_1, F_CPU_8, F_CPU_64, F_CPU_256, F_CPU_1024, EXT_FALLING, EXT_RISING,
	F_T2S_1 = 1, F_T2S_8 = 2, F_T2S_32 = 3, F_T2S_64 = 4, F_T2S_128 = 5, F_T2S_256 = 6, F_T2S_1024 = 7

} TIMERS_Clock;

typedef enum {
	DISCONNECT_OC, TOGGLE_OC, CLEAR_OC, SET_OC,
	NON_INVERTING = 2, INVERTING = 3,
	CLEAR_UP_SET_DOWN = 2, SET_UP_CLEAR_DOWN = 3
} TIMERS_Compare;

typedef struct {
	TIMERS_Num num;
	TIMERS_Mode mode;
	TIMERS_Clock clock;
	TIMERS_Compare compare;
	uint16 initial_value;
	uint16 compare_value;
} TIMERS_ConfigType;


/* Initialize a TIMER module */
void TIMERS_init(const TIMERS_ConfigType * const config_ptr);

/* Set the Call Back function address */
void TIMERS_setCallBack(TIMERS_Num num, TIMERS_Mode mode, void (*f_ptr)(void));

/* Disable a TIMER module */
void TIMERS_deInit(TIMERS_Num num);


#endif
