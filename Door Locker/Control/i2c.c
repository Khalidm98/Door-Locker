/* Driver for Atmega16 I2C (TWI) module */

#include "i2c.h"


void TWI_init(void) {
	/* Initialize TWBR Register:
	 * TWBR7:0 = 2				SCL division factor (400 kbps)
	 */
	TWBR = 2;

	/* Initialize TWSR Register:
	 * TWS7:3  = 00000			TWI status
	 * reserved
	 * TWPS1:0 = 00				SCL prescaler (400 kbps)
	 */
	TWSR = 0x00;
	
	/* Initialize TWAR Register:
	 * TWA6:0 = TWI_ADDRESS		set device address in case it is a slave
	 * TWGCE  = 0				disable generic call recognition
	 */
	TWAR = (TWI_ADDRESS << 1);
	
	/* Initialize TWCR Register:
	 * TWINT = 0				TWI interrupt flag
	 * TWEA  = 0				TWI enable acknowledge bit
	 * TWSTA = 0				TWI send start bit
	 * TWSTO = 0				TWI send stop bit
	 * TWWC  = 0				TWI write collision flag
	 * TWEN  = 1				enable TWI module
	 * reserved
	 * TWIE  = 0				disable TWI interrupts
	 */
	TWCR = (1 << TWEN);
}

void TWI_start(void) {
	/* Clear the TWINT flag before sending the start bit TWINT=1
	 * Send the start bit TWSTA=1
	 * Enable TWI Module TWEN=1 
	 */
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);

	/* Wait for TWINT flag set in TWCR Register (start bit is sent successfully) */
	while(BIT_IS_CLEAR(TWCR,TWINT));
}

void TWI_stop(void) {
	/* Clear the TWINT flag before sending the stop bit TWINT=1
	 * send the stop bit TWSTO=1
	 * Enable TWI Module TWEN=1 
	 */
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}

void TWI_write(uint8 data) {
	/* Put data in TWI data register */
	TWDR = data;

	/* Clear the TWINT flag before sending the data TWINT=1
	 * Enable TWI Module TWEN=1 
	 */ 
	TWCR = (1 << TWINT) | (1 << TWEN);

	/* Wait for TWINT flag set in TWCR Register (data is sent successfully) */
	while(BIT_IS_CLEAR(TWCR,TWINT));
}

uint8 TWI_readWithACK(void) {
	/* Clear the TWINT flag before reading the data TWINT=1
	 * Enable sending ACK after reading or receiving data TWEA=1
	 * Enable TWI Module TWEN=1 
	 */ 
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);

	/* Wait for TWINT flag set in TWCR Register (data is received successfully) */
	while(BIT_IS_CLEAR(TWCR,TWINT));

	/* Read Data */
	return TWDR;
}

uint8 TWI_readWithNACK(void) {
	/* 
	 * Clear the TWINT flag before reading the data TWINT=1
	 * Enable TWI Module TWEN=1 
	 */
	TWCR = (1 << TWINT) | (1 << TWEN);
	/* Wait for TWINT flag set in TWCR Register (data is received successfully) */
	while(BIT_IS_CLEAR(TWCR,TWINT));

	/* Read Data */
	return TWDR;
}

uint8 TWI_getStatus(void) {
	/* masking to eliminate first 3 bits and get the last 5 bits (status bits) */
	return (TWSR & 0xF8);
}
