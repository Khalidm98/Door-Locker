/* Driver for LCD (2x16 or 4x16) (4-bit or 8-bit) */

#include "lcd.h"


#if (DATA_BITS_MODE == 4)
#define DATA_PINS (0x0F<<PINS_POSITION)		/* 8-bits with HIGH at data pins position */
#endif


static void LCD_sendData(const uint8 data, const bool type);


void LCD_init(void) {
	/* Configure the control pins (RS,RW,E) as output pins */
	LCD_CTRL_DIR |= (1<<RS) | (1<<RW) | (1<<E);
	
	#if (DATA_BITS_MODE == 4)
		/* initialize LCD in 4-bit mode */
		LCD_sendCommand(FOUR_BITS_DATA_MODE);
		/* use 2-line lcd + 4-bit Data Mode + 5*7 dot display Mode */
		LCD_sendCommand(TWO_LINE_FOUR_BIT_MODE);
	#elif (DATA_BITS_MODE == 8)
		/* use 2-line lcd + 8-bit Data Mode + 5*7 dot display Mode */
		LCD_sendCommand(TWO_LINE_EIGHT_BIT_MODE);
	#endif
	
	LCD_sendCommand(DISPLAY_ON_CURSOR_OFF); 	/* turn on display */
	LCD_clearScreen();							/* clear the LCD */
}

void LCD_clearScreen(void) {
	LCD_sendCommand(CLEAR_SCREEN);				/* clear display */
}

void LCD_sendCommand(const uint8 command) {
	LCD_sendData(command, 0);
}

void LCD_displayCharacter(const uint8 character) {
	LCD_sendData(character, 1);
}

static void LCD_sendData(const uint8 data, const bool type) {
	#if (DATA_BITS_MODE == 4)
		/* Configure 4 adjacent pins of the data port as output pins */
		LCD_DATA_DIR |= DATA_PINS;
	#elif (DATA_BITS_MODE == 8)
		/* Configure the data port as output port */
		LCD_DATA_DIR = 0xFF;
	#endif

	/* data is command -> RS = 0 OR data is character -> RS = 1 */
	type == 0 ? CLEAR_BIT(LCD_CTRL_OUT,RS) : SET_BIT(LCD_CTRL_OUT,RS);
	CLEAR_BIT(LCD_CTRL_OUT,RW);		/* write data to LCD -> RW = 0 */
	_delay_ms(1);					/* delay for processing Tas = 50ns */
	SET_BIT(LCD_CTRL_OUT,E);		/* Enable LCD -> E = 1 */
	_delay_ms(1);					/* delay for processing = Tpw - Tdws = 190ns */
	
	#if (DATA_BITS_MODE == 4)
		/* out the highest 4 bits of the required data to the data bus D4 --> D7 */
		LCD_DATA_OUT = (LCD_DATA_OUT & ~DATA_PINS) | ((data & 0xF0) >> (4-PINS_POSITION));
		_delay_ms(1);					/* delay for processing Tdsw = 100ns */
		CLEAR_BIT(LCD_CTRL_OUT,E);		/* disable LCD -> E = 0 */
		_delay_ms(1);					/* delay for processing Th = 13ns */
		
		SET_BIT(LCD_CTRL_OUT,E);		/* Enable LCD -> E = 1 */
		_delay_ms(1);					/* delay for processing = Tpw - Tdws = 190ns */

		/* out the lowest 4 bits of the required data to the data bus D4 --> D7 */
		LCD_DATA_OUT = (LCD_DATA_OUT & ~DATA_PINS) | ((data & 0x0F) << PINS_POSITION);
		_delay_ms(1);					/* delay for processing Tdsw = 100ns */
		CLEAR_BIT(LCD_CTRL_OUT,E);		/* disable LCD -> E = 0 */
		_delay_ms(1);					/* delay for processing Th = 13ns */

	#elif (DATA_BITS_MODE == 8)
		/* out the required data to the data bus D0 --> D7 */
		LCD_DATA_OUT = data;
		_delay_ms(1);						/* delay for processing Tdsw = 100ns */
		CLEAR_BIT(LCD_CTRL_OUT,E);			/* disable LCD -> E = 0 */
		_delay_ms(1);						/* delay for processing Th = 13ns */
	#endif
}

void LCD_displayString(const char *str) {
	while ((*str) != '\0') {
		LCD_displayCharacter(*str);
		str++;
	}
}

void LCD_moveCursorTo(const uint8 row, const uint8 col) {
	uint8 address;
	/* calculate the required address */
	switch(row) {
		case 0:		address = col;				break;
		case 1:		address = col + 0x40;		break;
		case 2:		address = col + 0x10;		break;
		case 3:		address = col + 0x50;		break;
	}					
	/* to write to a specific address in the LCD
	 * we need to apply the corresponding command
	 * 0x80 + address OR 0x80 | address (no difference)
	 * Max Address = 0x5F = 0b0101.1111 AND 0x80 = 0b1000.0000*/
	LCD_sendCommand(address | SET_CURSOR_LOCATION);
}

void LCD_displayStringAt(const uint8 row, const uint8 col, const char *str) {
	LCD_moveCursorTo(row,col);		/* go to to the required LCD position */
	LCD_displayString(str);			/* display the string */
}

void LCD_displayInteger(const sint32 data) {
   char buff[16];					/* string to hold the ASCII result */
   itoa(data, buff, 10);			/* 10 for decimal (base 10) */
   LCD_displayString(buff);			/* display the string */
}
