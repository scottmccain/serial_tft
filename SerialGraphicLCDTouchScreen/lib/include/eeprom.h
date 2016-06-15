/*
 * eeprom.h
 *
 * Created: 6/14/2016 5:55:14 PM
 *  Author: Monkey
 */ 


#ifndef EEPROM_H_
#define EEPROM_H_

#include <stdint.h>

//EEprom addresses
#define EE_BPS 0
#define EE_BACKLIGHT 1
#define EE_SPLASH 2
#define EE_REV 3
#define EE_DEBUG 4
#define EE_CRLF 5


//functions that write to flash
inline void EEPROM_write(uint16_t uiAddress, uint8_t ucData);
inline uint8_t EEPROM_read(uint16_t uiAddress);




#endif /* EEPROM_H_ */