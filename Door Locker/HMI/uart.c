/* Driver for Atmega16 UART module */

#include "uart.h"

#ifndef ASYNC
#define BAUD_PRESCALE ((F_CPU / (USART_BAUDRATE * 2UL)) - 1)	
#else
	#ifdef UART_DOUBLE_SPEED
	#define BAUD_PRESCALE ((F_CPU / (USART_BAUDRATE * 8UL)) - 1)	
	#else
	#define BAUD_PRESCALE ((F_CPU / (USART_BAUDRATE * 16UL)) - 1)	
	#endif
#endif


void UART_init(const UART_ConfigType * const config_ptr) {
	/* Initialize UCSRA Register:
	 * RXC  = 0			receive complete flag
	 * TXC  = 0			transmit complete flag
	 * UDRE = 0			data register empty flag
	 * FE   = 0			frame error flag
	 * DOR  = 0			data overrun error flag
	 * PE   = 0			parity error flag
	 * U2X  = x			double transmission speed
	 * MPCM = 0			disable multi-processor communication mode
	 */

	/* select transmission speed */
	#ifdef UART_DOUBLE_SPEED
		UCSRA = (1<<U2X);
	#else
		UCSRA = 0;
	#endif

	/* Initialize UCSRB Register:
	 * RXCIE = 0		disable receive complete interrupt
	 * TXCIE = 0		disable transmit complete interrupt
	 * UDRIE = 0		disable data register empty interrupt
	 * RXEN  = 1		enable receiver
	 * TXEN  = 1		enable transmitter
	 * UCSZ2 = 0		disable 9-bit data mode
	 * RXB8  = 0		disable 9-bit data mode
	 * TXB8  = 0		disable 9-bit data mode
	 */
	
	/* enable receiver and transmitter */
	UCSRB = (1<<RXEN) | (1<<TXEN);
	
	/* Initialize UCSRC Register:
	 * URSEL   = 1		URSEL must be one when writing to UCSRC
	 * UMSEL   = x		USART mode
	 * UPM1:0  = xx		parity bit
	 * USBS    = x		stop bit
	 * UCSZ1:0 = xx		number of data bits
	 * UCPOL   = x		UCPOL must be zero when using asynchronous mode
	 */

	/* write to UCSRC */
	UCSRC = (1<<URSEL);

	/* select UART mode */
	#ifndef ASYNC
		SET_BIT(UCSRC,UMSEL);
	#endif

	/* set parity mode */
	UCSRC = (UCSRC & 0xCF) | (config_ptr->parity << 4);

	/* select stop bits number */
	if (config_ptr->stop)
		SET_BIT(UCSRC,USBS);

	/* set size of data bits */
	UCSRC = (UCSRC & 0xF9) | (config_ptr->size << 1);

	/* select clock polarity */
	#ifdef TX_FALLING_RX_RISING
		SET_BIT(UCSRC,UCPOL);
	#endif
	
	/* set the UBRR to select the Baud Rate */
	UBRRH = BAUD_PRESCALE >> 8;
	UBRRL = BAUD_PRESCALE;
}

void UART_sendByte(const uint8 data) {
	/* UDRE flag is set when the Tx buffer (UDR) is empty and ready for
	 * transmitting a new byte so wait until this flag is set to one */
	while(BIT_IS_CLEAR(UCSRA,UDRE));
	/* Put the required data in the UDR register and also clear the UDRE flag
	 * as the UDR register is not empty now */
	UDR = data;
	
	/* Another Slower Method
	UDR = data;
	while(BIT_IS_CLEAR(UCSRA,TXC));		// Wait until the transimission is complete
	SET_BIT(UCSRA,TXC);					// Clear the TXC flag
	*/
}

uint8 UART_receiveByte(void) {
	/* RXC flag is set when the UART receives data */
	while(BIT_IS_CLEAR(UCSRA,RXC));
	/* Read the received data from the Rx buffer (UDR)
	 * the RXC flag will be cleared after reading UDR */
    return UDR;
}

void UART_sendString(const uint8 *str) {
	while(*str != '\0') {
		UART_sendByte(*str);
		str++;
	}
}

void UART_receiveString(uint8 *str) {
	uint8 i = 0;
	str[i] = UART_receiveByte();
	while(str[i] != UART_TERMINATION_CHAR) {
		i++;
		str[i] = UART_receiveByte();
	}
	str[i] = '\0';
}
