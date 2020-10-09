/* Driver for LCD (2x16 or 4x16) (4-bit or 8-bit) */

#ifndef LCD_H_
#define LCD_H_


#include <stdlib.h>
#include "std_types.h"
#include "micro_config.h"
#include "common_macros.h"


/* LCD data bits mode configuration */
#define DATA_BITS_MODE 8

/* LCD HW Pins */
#if (DATA_BITS_MODE == 4)		/* Use higher 4 bits in the data port */
#define PINS_POSITION 0			/* (0 -> 4) select 4 adjacent pins for LCD data port */
#endif
#define LCD_DATA_DIR DDRC
#define LCD_DATA_IN	 PINC
#define LCD_DATA_OUT PORTC

#define RS PA2
#define RW PA1
#define E  PA0
#define LCD_CTRL_DIR DDRA
#define LCD_CTRL_OUT PORTA

/* LCD Commands */
#define CLEAR_SCREEN 0x01
#define FOUR_BITS_DATA_MODE 0x02
#define TWO_LINE_FOUR_BIT_MODE 0x28
#define TWO_LINE_EIGHT_BIT_MODE 0x38
#define DISPLAY_ON_CURSOR_BLINK 0x0F
#define DISPLAY_ON_CURSOR_OFF 0x0C
#define DISPLAY_ON_CURSOR_ON 0x0E
#define SET_CURSOR_LOCATION 0x80 


/* Functions Prototypes */
void LCD_init(void);
void LCD_clearScreen(void);
void LCD_sendCommand(const uint8 command);
void LCD_displayCharacter(const uint8 character);
void LCD_displayString(const char *str);
void LCD_displayInteger(const sint32 data);
void LCD_moveCursorTo(const uint8 row, const uint8 col);
void LCD_displayStringAt(const uint8 row, const uint8 col, const char *str);


#endif /* LCD_H_ */
