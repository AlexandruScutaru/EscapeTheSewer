#include "level_loader.h"
#include "level.h"
#include "vec2.inl"
#include "eeprom_utils.h"
#include "level.h"

#include <Arduino.h>

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


bool LevelLoader::loadLevel(uint8_t index, Level& level) {
    Serial.println("loadLevel");
    EepromUtils::beginWire();

    int currentAddress = 0;
    uint8_t currLevel = EepromUtils::readFromAddress(currentAddress++);
    uint8_t num_levels = EepromUtils::readFromAddress(currentAddress++);
    
    Serial.println(index);
    Serial.println(num_levels);

    if (index >= num_levels) {
        Serial.println("not possible");
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

    Serial.print("w: "); Serial.print(level.levelW); Serial.print(" h: "); Serial.println(level.levelH);

    for (int i = 0; i < level.levelH; i++) {
        for (int j = 0; j < level.levelW; j++) {
            level.levelData[i][j].packed = EepromUtils::readFromAddress(currentAddress++);
        }
    }

    level.startCoords.x = static_cast<float>(readWord(currentAddress));
    level.startCoords.y = static_cast<float>(readWord(currentAddress));

    level.endCoords.x = static_cast<float>(readWord(currentAddress));
    level.endCoords.y = static_cast<float>(readWord(currentAddress));

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

    Serial.println(count);
    EepromUtils::endWire();

    return true;
}

bool LevelLoader::writeCurrentLevelIndex() {
    //SdFat SD;
    //if (!SD.begin(SD_CS)) {
    //    while (1);
    //}
    //
    //File file;
    //file = SD.open("levels.bin", O_WRITE | O_READ | O_CREAT);
    //2x
    //if (!file) {
    //    return false;
    //}
    //
    //file.seek(0);
    //file.write(reinterpret_cast<char*>(&mCurrentLevel), sizeof(uint8_t));
    //file.close();

    return true;
}
