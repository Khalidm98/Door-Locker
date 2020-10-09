/* Driver for EEPROM (24Cxx) series */

#ifndef EXTERNAL_EEPROM_H_
#define EXTERNAL_EEPROM_H_


#include "std_types.h"
#include "micro_config.h"
#include "common_macros.h"


#define ERROR 0
#define SUCCESS 1


void EEPROM_init(void);
uint8 EEPROM_writeByte(const uint16 address, const uint8 data);
uint8 EEPROM_readByte(const uint16 address, uint8 * const data);

 
#endif /* EXTERNAL_EEPROM_H_ */
