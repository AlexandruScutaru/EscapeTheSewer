#include "eeprom_utils.h"

#include <Arduino.h>
#include <Wire.h>


namespace EepromUtils {

    namespace {
        void handleError() {
            for (;;) {
                //do something like a LED blinking pattern
            }
        }

        void setAddress(uint16_t address) {
            Wire.write(uint8_t(address >> 8));
            Wire.write(uint8_t(address & 0xFF));
        }
    }

    void beginWire() {
        Wire.begin();
    }

    void endWire() {
        Wire.end();
    }

    void writeAtAddress(uint16_t address, uint8_t value, int deviceAddress) {
        Wire.beginTransmission(deviceAddress);
        setAddress(address);
        Wire.write(value);

        if (Wire.endTransmission())
            handleError();

        delay(5);
    }

    uint8_t readFromAddress(uint16_t address, int deviceAddress) {
        Wire.beginTransmission(deviceAddress);
        setAddress(address);

        if (Wire.endTransmission())
            handleError();

        if (!Wire.requestFrom(deviceAddress, 1))
            handleError();

        uint8_t value = 0xFF;
        while (Wire.available())
            value = Wire.read();

        return value;
    }

}
