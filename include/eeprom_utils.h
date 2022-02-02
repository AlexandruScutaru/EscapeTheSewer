#ifndef EEPROM_UTILS_H
#define EEPROM_UTILS_H

#include <stdint.h>

namespace EepromUtils {
    void beginWire();
    void endWire();
    void writeAtAddress(uint16_t address, uint8_t value, int deviceAddress = 0x51);
    uint8_t readFromAddress(uint16_t address, int deviceAddress = 0x51);
}

#endif // EEPROM_UTILS_H
