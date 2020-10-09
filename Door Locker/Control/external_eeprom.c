/* Driver for EEPROM (24Cxx) series */

#include "i2c.h"
#include "external_eeprom.h"


void EEPROM_init(void) {
	/* just initialize the I2C(TWI) module inside the MC */
	TWI_init();
}

uint8 EEPROM_writeByte(const uint16 address, const uint8 data) {
	/* Send the Start Bit */
    TWI_start();
    if (TWI_getStatus() != TW_START)
        return ERROR;
		
    /* Device address = 1010 + upper 3 bits of memory location address
     * write operation so R/W=0 */
    TWI_write((uint8)(0xA0 | ((address & 0x0700) >> 7)));
    if (TWI_getStatus() != TW_MT_SLA_W_ACK)
        return ERROR; 
		 
    /* Send the remaining memory location address */
    TWI_write((uint8)(address));
    if (TWI_getStatus() != TW_MT_DATA_ACK)
        return ERROR;
		
    /* write byte to EEPROM */
    TWI_write(data);
    if (TWI_getStatus() != TW_MT_DATA_ACK)
        return ERROR;

    /* Send the Stop Bit */
    TWI_stop();
    return SUCCESS;
}

uint8 EEPROM_readByte(const uint16 address, uint8 * const data) {
	/* Send the Start Bit */
    TWI_start();
    if (TWI_getStatus() != TW_START)
        return ERROR;
		
    /* Device address = 1010 + upper 3 bits of memory location address
     * write operation so R/W=0 */
    TWI_write((uint8)((0xA0) | ((address & 0x0700) >> 7)));
    if (TWI_getStatus() != TW_MT_SLA_W_ACK)
        return ERROR;
		
    /* Send the required memory location address */
    TWI_write((uint8)(address));
    if (TWI_getStatus() != TW_MT_DATA_ACK)
        return ERROR;
		
    /* Send the Repeated Start Bit */
    TWI_start();
    if (TWI_getStatus() != TW_REP_START)
        return ERROR;
		
    /* Device address = 1010 + upper 3 bits of memory location address
     * read operation so R/W=1 */
    TWI_write((uint8)((0xA1) | ((address & 0x0700) >> 7)));
    if (TWI_getStatus() != TW_MT_SLA_R_ACK)
        return ERROR;

    /* Read Byte from EEPROM without sending an ACK */
    *data = TWI_readWithNACK();
    if (TWI_getStatus() != TW_MR_DATA_NACK)
        return ERROR;

    /* Send the Stop Bit */
    TWI_stop();
    return SUCCESS;
}
