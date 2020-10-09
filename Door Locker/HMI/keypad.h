/* Driver for keypad (4x3 or 4x4) */

#ifndef KEYPAD_H_
#define KEYPAD_H_


#include "std_types.h"
#include "micro_config.h"
#include "common_macros.h"


/* Keypad configurations for number of rows and columns */
#define N_COL 3
#define N_ROW 4

/* Keypad Port Configurations */
#define KEYPAD_PORT_DIR DDRB
#define KEYPAD_PORT_IN  PINB
#define KEYPAD_PORT_OUT PORTB


/* Get the pressed keypad key */
uint8 Keypad_getPressedKey(void);


#endif /* KEYPAD_H_ */
