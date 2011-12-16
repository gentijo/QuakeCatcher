#pragma once

#include <avr/eeprom.h>

/* macros to set and clear bits in registers */
#define SetBit(sfr, bit) ((sfr) |= (1 << (bit)))
#define ClearBit(sfr, bit) ((sfr) &= ~(1 << (bit)))

/* macros to write to and read from EEPROM */
#define EEPROMWriteByte(addr, byte) \
			eeprom_write_byte((uint8_t *)(addr), (byte))
#define EEPROMReadByte(addr) \
			eeprom_read_byte((uint8_t *)(addr))
