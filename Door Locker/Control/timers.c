/* Driver for Atmega16 TIMERS modules */

#include "timers.h"


/* Global pointers to functions holding the address of the call back function of each timer mode */
static volatile void (*g_timer0_overflow)(void) = NULL_PTR;
static volatile void (*g_timer0_compare)(void) = NULL_PTR;
static volatile void (*g_timer1_overflow)(void) = NULL_PTR;
static volatile void (*g_timer1_compareA)(void) = NULL_PTR;
static volatile void (*g_timer1_compareB)(void) = NULL_PTR;
static volatile void (*g_timer2_overflow)(void) = NULL_PTR;
static volatile void (*g_timer2_compare)(void) = NULL_PTR;


/* Interrupt Sevice Routines of all timers modules and modes */
ISR(TIMER0_OVF_vect) {
	if (g_timer0_overflow != NULL_PTR)
		(*g_timer0_overflow)();
}

ISR(TIMER0_COMP_vect) {
	if (g_timer0_compare != NULL_PTR)
		(*g_timer0_compare)();
}

ISR(TIMER1_OVF_vect) {
	if (g_timer1_overflow != NULL_PTR)
		(*g_timer1_overflow)();
}

ISR(TIMER1_COMPA_vect) {
	if (g_timer1_compareA != NULL_PTR)
		(*g_timer1_compareA)();
}

ISR(TIMER1_COMPB_vect) {
	if (g_timer1_compareB != NULL_PTR)
		(*g_timer1_compareB)();
}

ISR(TIMER2_OVF_vect) {
	if (g_timer2_overflow != NULL_PTR)
		(*g_timer2_overflow)();
}

ISR(TIMER2_COMP_vect) {
	if (g_timer2_compare != NULL_PTR)
		(*g_timer2_compare)();
}


void TIMERS_init(const TIMERS_ConfigType * const config_ptr) {
	/* Initialize TIMSK Register:
	 * OCIE2  = x		TIMER2 output compare match interrupt enable
	 * TOIE2  = x		TIMER2 overflow interrupt enable
	 * TICIE1 = x		TIMER1 input capture interrupt enable
	 * OCIE1A = x		TIMER1 output compare A match interrupt enable
	 * OCIE1B = x		TIMER1 output compare B match interrupt enable
	 * TOIE1  = x		TIMER1 overflow interrupt enable
	 * OCIE0  = x		TIMER0 output compare match interrupt enable
	 * TOIE0  = x		TIMER0 overflow interrupt enable
	 */

	if (config_ptr->num == TIMER1 || config_ptr->num == TIMER1A || config_ptr->num == TIMER1B) {
		/* Initialize TCCR1A Register:
		 * COM1A1:0 = xx	compare match output mode for channel A
		 * COM1B1:0 = xx	compare match output mode for channel B
		 * FOC1A    = x		force output compare for channel A
		 * FOC1B    = x		force output compare for channel B
		 * WGM11:0  = xx	waveform generation mode
		 */

		/* set waveform generation mode */
		TCCR1A = (config_ptr->mode & 0x03);

		/* enable / disable force output compare */
		if (config_ptr->mode == NORMAL || config_ptr->mode == CTC_OCR1A || config_ptr->mode == CTC_ICR1) {
			if (config_ptr->num == TIMER1)
				TCCR1A |= 0x0C;
			else if (config_ptr->num == TIMER1A)
				SET_BIT(TCCR1A,FOC1A);
			else if (config_ptr->num == TIMER1B)
				SET_BIT(TCCR1A,FOC1B);
		}

		/* set compare match output mode */
		if (config_ptr->compare != DISCONNECT_OC){
			if (config_ptr->num == TIMER1) {
				TCCR1A |= (config_ptr->compare << 4) | (config_ptr->compare << 6);
				DDRD |= 0x30;				/* set pins OC1A and OC1B as output pins */
			}

			else if (config_ptr->num == TIMER1A) {
				TCCR1A |= (config_ptr->compare << 6);
				SET_BIT(DDRD,PD5);			/* set pin OC1A (PD5) as output pin */
			}

			else if (config_ptr->num == TIMER1B) {
				TCCR1A |= (config_ptr->compare << 4);
				SET_BIT(DDRD,PD4);			/* set pin OC1B (PD4) as output pin */
			}

		}


		/* Initialize TCCR1B Register:
		 * ICNC1   = 0		disable input capture noise canceler
		 * ICES1   = 0		trigger input capture at falling edge (trivial)
		 * reserved
		 * WGM13:2 = xx		waveform generation mode
		 * CS12:0  = xxx	clock select
		 */

		/* set clock prescaler */
		TCCR1B = config_ptr->clock;

		/* set waveform generation mode */
		TCCR1B = (TCCR1B & 0xE7) | ((config_ptr->mode & 0x0C) << 1);


		/* set initial value */
		TCNT1 = config_ptr->initial_value;

		/* set compare value */
		if (config_ptr->num == TIMER1) {
			OCR1A = config_ptr->compare_value;
			OCR1B = config_ptr->compare_value;
		}
		else if (config_ptr->num == TIMER1A)
			OCR1A = config_ptr->compare_value;
		else if (config_ptr->num == TIMER1B)
			OCR1B = config_ptr->compare_value;

		/* enable interrupt */
		if (config_ptr->num == TIMER1)
			SET_BIT(TIMSK,TOIE1);
		else if (config_ptr->num == TIMER1A)
			SET_BIT(TIMSK,OCIE1A);
		else if (config_ptr->num == TIMER1B)
			SET_BIT(TIMSK,OCIE1B);
	}

	else if (config_ptr->num == TIMER0) {
		/* Initialize TCCR0 Register:
		 * FOC0    = x		force output compare
		 * WGM00   = x		waveform generation mode
		 * COM01:0 = xx		compare match output mode
		 * WGM01   = x		waveform generation mode
		 * CS02:0  = xxx	clock select
		 */

		/* set clock prescaler */
		TCCR0 = config_ptr->clock;

		/* set waveform generation mode */
		if (config_ptr->mode == PWM_PHASE_CORRECT)
			SET_BIT(TCCR0,WGM00);
		else if (config_ptr->mode == CTC)
			SET_BIT(TCCR0,WGM01);
		else if (config_ptr->mode == FAST_PWM)
			TCCR0 |= 0x48;
		
		/* set compare match output mode */
		if (config_ptr->compare != DISCONNECT_OC) {
			TCCR0 |= (config_ptr->compare << 4);
			SET_BIT(DDRB,PB3);			/* set pin OC0 (PB3) as output pin */
		}
		
		/* enable / disable force output compare */
		if (config_ptr->mode == NORMAL || config_ptr->mode == CTC)
			SET_BIT(TCCR0,FOC0);


		/* set initial value */
		TCNT0 = config_ptr->initial_value;

		/* set compare value */
		OCR0 = config_ptr->compare_value;
		
		/* enable interrupt */
		if (config_ptr->mode == NORMAL)
			SET_BIT(TIMSK,TOIE0);
		else if (config_ptr->mode == CTC)
			SET_BIT(TIMSK,OCIE0);
	}

	else if (config_ptr->num == TIMER2) {
		/* Initialize TCCR2 Register:
		 * FOC2    = x		force output compare
		 * WGM20   = x		waveform generation mode
		 * COM21:0 = xx		compare match output mode
		 * WGM21   = x		waveform generation mode
		 * CS22:0  = xxx	clock select
		 */

		/* set clock prescaler */
		TCCR2 = config_ptr->clock;

		/* set waveform generation mode */
		if (config_ptr->mode == PWM_PHASE_CORRECT)
			SET_BIT(TCCR2,WGM20);
		else if (config_ptr->mode == CTC)
			SET_BIT(TCCR2,WGM21);
		else if (config_ptr->mode == FAST_PWM)
			TCCR2 |= 0x48;
		
		/* set compare match output mode */
		if (config_ptr->compare != DISCONNECT_OC) {
			TCCR2 |= (config_ptr->compare << 4);
			SET_BIT(DDRD,PD7);			/* set pin OC2 (PD7) as output pin */
		}
		
		/* enable / disable force output compare */
		if (config_ptr->mode == NORMAL || config_ptr->mode == CTC)
			SET_BIT(TCCR2,FOC2);


		/* set initial value */
		TCNT2 = config_ptr->initial_value;

		/* set compare value */
		OCR2 = config_ptr->compare_value;
		
		/* enable interrupt */
		if (config_ptr->mode == NORMAL)
			SET_BIT(TIMSK,TOIE2);
		else if (config_ptr->mode == CTC)
			SET_BIT(TIMSK,OCIE2);
	}
}

void TIMERS_setCallBack(TIMERS_Num num, TIMERS_Mode mode, void (*f_ptr)(void)) {
	if (num == TIMER1)
		g_timer1_overflow = f_ptr;
	else if (num == TIMER1A)
		g_timer1_compareA = f_ptr;
	else if (num == TIMER1B)
		g_timer1_compareB = f_ptr;
	
	else if (num == TIMER0) {
		if (mode == NORMAL)
			g_timer0_overflow = f_ptr;
		if (mode == CTC)
			g_timer0_compare = f_ptr;	
	}

	else if (num == TIMER2) {
		if (mode == NORMAL)
			g_timer2_overflow = f_ptr;
		if (mode == CTC)
			g_timer2_compare = f_ptr;	
	}
}

void TIMERS_deInit(TIMERS_Num num){
	/* disable the clock to disable the timer */
	if (num == TIMER1 || num == TIMER1A || num == TIMER1B)
		TCCR1B = 0;
	else if (num == TIMER0)
		TCCR0 = 0;
	else if (num == TIMER2)
		TCCR2 = 0;
}
