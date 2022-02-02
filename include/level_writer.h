#include <Arduino.h>
#include <Wire.h>

#include "eeprom_utils.h"

// this file hold only this method to upload the python generated byte array with the levels to the EEPROM

#include "levels.h"


void WriteLevel() {
    EepromUtils::beginWire();

    for (uint16_t i = 0; i < ARRAY_SIZE; i++) {
        EepromUtils::writeAtAddress(i, pgm_read_byte(&levels[i]));
    }

    EepromUtils::endWire();
}