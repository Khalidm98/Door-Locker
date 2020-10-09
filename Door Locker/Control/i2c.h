/* Driver for Atmega16 I2C (TWI) module */

/* Constraints:
 * Supports polling only (no iterrupts)
 * communication speed = 400 kbps always
 * generic call recognition is disabled
 */


#ifndef I2C_H_
#define I2C_H_


#include "std_types.h"
#include "micro_config.h"
#include "common_macros.h"


#define TWI_ADDRESS 1


/* I2C Status Bits in the TWSR Register */
#define TW_START		0x08	/* start has been sent */
#define TW_REP_START	0x10	/* repeated start */
#define TW_MT_SLA_W_ACK	0x18	/* Master transmit (slave address + Write request) + ACK received from slave */
#define TW_MT_SLA_R_ACK	0x40	/* Master transmit (slave address + Read request) + ACK received from slave */
#define TW_MT_DATA_ACK	0x28	/* Master transmit data + ACK received from slave */
#define TW_MR_DATA_ACK	0x50	/* Master received data + master sent ACK to slave */
#define TW_MR_DATA_NACK	0x58	/* Master received data + master didn't send ACK to slave */


/* Initialize the TWI module */
void TWI_init(void);

/* Send a start bit */
void TWI_start(void);

/* Send a stop bit */
void TWI_stop(void);

/* Send data */
void TWI_write(uint8 data);

/* Read then send an ACK */
uint8 TWI_readWithACK(void);

/* Read then send a NACK */
uint8 TWI_readWithNACK(void);

/* Get the TWI status */
uint8 TWI_getStatus(void);


#endif /* I2C_H_ */
