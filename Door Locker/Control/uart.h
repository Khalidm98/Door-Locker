/* Driver for Atmega16 UART module */

/* Constraints:
 * Supports polling only (no iterrupts)
 * Multi-processor communication mode is always disabled
 * Both RX and TX are always enabled
 * 9-bit mode is always disabled (only 5,6,7,8)
 */


#ifndef UART_H_
#define UART_H_


#include "std_types.h"
#include "micro_config.h"
#include "common_macros.h"


/* UART Driver Baud Rate */
#define USART_BAUDRATE 9600

/* Asynchronous mode */
#define ASYNC						/* (undefine / comment) this when using synchrounous mode */

#ifdef ASYNC
#define UART_DOUBLE_SPEED			/* (undefine / comment) this for normal speed */
#else
#define TX_FALLING_RX_RISING		/* (undefine / comment) this for opposite behaviour */
#endif

#define UART_TERMINATION_CHAR '#'	/* a character that marks the end of a string */

typedef enum {
	ONE_BIT, TWO_BITS
} UART_StopBit;

typedef enum {
	DISABLE = 0, EVEN = 2, ODD = 3
} UART_ParityMode;

typedef enum {
	BIT_5, BIT_6, BIT_7, BIT_8
} UART_CharacterSize;

typedef struct {
	UART_StopBit stop;
	UART_ParityMode parity;
	UART_CharacterSize size;
} UART_ConfigType;


/* Initialize the UART module */
void UART_init(const UART_ConfigType * const config_ptr);

/* Send a byte using UART TX */
void UART_sendByte(const uint8 data);

/* Receive a byte using UART RX */
uint8 UART_receiveByte(void);

/* Send multiple bytes using UART TX */
void UART_sendString(const uint8 *str);

/* Receive multiple bytes using UART RX */
void UART_receiveString(uint8 *str);


#endif /* UART_H_ */
