#include "level_loader.h"
#include "level.h"
#include "vec2.inl"
#include "eeprom_utils.h"
#include "level.h"

#include <Arduino.h>

namespace LevelLoader {

    namespace {
        static uint16_t readWord(int& address) {
            uint8_t wordValue = 0;
            uint16_t outValue = 0;
            wordValue = EepromUtils::readFromAddress(address++); outValue |= (uint32_t)wordValue <<  0;
            wordValue = EepromUtils::readFromAddress(address++); outValue |= (uint32_t)wordValue <<  8;
            return outValue;
        }

        static uint32_t readDWord(int& address) {
            uint8_t wordValue = 0;
            uint32_t outValue = 0;
            wordValue = EepromUtils::readFromAddress(address++); outValue |= (uint32_t)wordValue <<  0;
            wordValue = EepromUtils::readFromAddress(address++); outValue |= (uint32_t)wordValue <<  8;
            wordValue = EepromUtils::readFromAddress(address++); outValue |= (uint32_t)wordValue << 16;
            wordValue = EepromUtils::readFromAddress(address++); outValue |= (uint32_t)wordValue << 24;
            return outValue;
        }
    }

    bool LoadLevel(uint8_t index, Level& level) {
        EepromUtils::beginWire();

        int currentAddress = 0;
        EepromUtils::readFromAddress(currentAddress++);
        uint8_t num_levels = EepromUtils::readFromAddress(currentAddress++);

        if (index >= num_levels) {
            EepromUtils::endWire();
            return false;
        }

        uint32_t offsetToLevelData = 0;
        for (int i = 0; i < index; i++) {
            offsetToLevelData += readDWord(currentAddress);
        }

        //skip next level sizes and past levels if applicable
        currentAddress += sizeof(uint32_t) * (num_levels - index) + offsetToLevelData;

        level.levelW = EepromUtils::readFromAddress(currentAddress++);
        level.levelH = EepromUtils::readFromAddress(currentAddress++);

        //Serial.print("w: "); Serial.print(level.levelW); Serial.print(" h: "); Serial.println(level.levelH);

        for (int i = 0; i < level.levelH; i++) {
            for (int j = 0; j < level.levelW; j++) {
                level.levelData[i][j].packed = EepromUtils::readFromAddress(currentAddress++);
            }
        }

        level.startCoords.x = static_cast<float>(readWord(currentAddress));
        level.startCoords.y = static_cast<float>(readWord(currentAddress));

        level.endCoords.x = static_cast<float>(readWord(currentAddress));
        level.endCoords.y = static_cast<float>(readWord(currentAddress));

        level.enemies.clear();
        level.pickups.clear();
        uint8_t count = 0;
        vec2 pos;
        count = EepromUtils::readFromAddress(currentAddress++);
        for (int i = 0; i < count; i++) {
            pos.x = static_cast<float>(readWord(currentAddress));
            pos.y = static_cast<float>(readWord(currentAddress));
            level.enemies.push_back(Enemy(pos, EnemyConfig::Type::SLIME));
        }

        count = EepromUtils::readFromAddress(currentAddress++);
        for (int i = 0; i < count; i++) {
            pos.x = static_cast<float>(readWord(currentAddress));
            pos.y = static_cast<float>(readWord(currentAddress));
            level.enemies.push_back(Enemy(pos, EnemyConfig::Type::BUG));
        }

        count = EepromUtils::readFromAddress(currentAddress++);
        for (int i = 0; i < count; i++) {
            pos.x = static_cast<float>(readWord(currentAddress));
            pos.y = static_cast<float>(readWord(currentAddress));
            level.pickups.push_back(Pickup(pos, 50, 0));
        }

        EepromUtils::endWire();

        return true;
    }

    bool writeCurrentLevelIndex() {
        return false;
    }

}
