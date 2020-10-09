/* Driver for keypad (4x3 or 4x4) */

#include "keypad.h"

/* Map the switch number in the keypad to its corresponding key */
#if (N_COL == 3)
static uint8 Keypad_4x3_adjustKeyNumber(uint8 button_number);
#elif (N_COL == 4)
static uint8 Keypad_4x4_adjustKeyNumber(uint8 button_number);
#endif

uint8 Keypad_getPressedKey(void) {
	uint8 col, row;
	/* loop until a key is pressed */
	while(1) {
		/* loop for columns */
		for (col = 0; col < N_COL; col++) {
			/* each time only one of the column pins will be output and
			 * the rest will be input pins including the row pins */
			KEYPAD_PORT_DIR = 0b00010000 << col;
			
			/* clear the output column pin and
			 * enable the internal pull up resistors for the rows pins*/
			KEYPAD_PORT_OUT = ~(0b00010000 << col);

			/* loop for rows */
			for (row = 0; row < N_ROW; row++) {
				/* if the switch is pressed in this row */
				if(BIT_IS_CLEAR(KEYPAD_PORT_IN,row)) {
					#if (N_COL == 3)
						return Keypad_4x3_adjustKeyNumber(row * N_COL + col + 1);
					#elif (N_COL == 4)
						return Keypad_4x4_adjustKeyNumber(row * N_COL + col + 1);
					#endif
				}
			}
		}
	}
}

#if (N_COL == 3)
static uint8 Keypad_4x3_adjustKeyNumber(uint8 button_number) {
	switch(button_number) {
		case 10:		return '*';
		case 11:		return 0;
		case 12:		return '#';
		default:		return button_number;
	}
}

#elif (N_COL == 4)
static uint8 Keypad_4x4_adjustKeyNumber(uint8 button_number) {
	switch(button_number) {
		case 1:			return 7;
		case 2:			return 8;
		case 3:			return 9;
		case 4:			return '%';
		case 5:			return 4;
		case 6:			return 5;
		case 7:			return 6;
		case 8:			return '*';
		case 9:			return 1;
		case 10:		return 2;
		case 11:		return 3;
		case 12:		return '-';
		case 13:		return 13;		/* ASCII of Enter */
		case 14:		return 0;
		case 15:		return '=';
		case 16:		return '+';
		default:		return button_number;
	}
}
#endif
