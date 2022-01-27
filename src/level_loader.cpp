#include "level_loader.h"
#include "level.h"
#include "vec2.inl"

#include <Arduino.h>
#include <SPI.h>

#define SD_CS 4


LevelLoader::LevelLoader() {
    pinMode(PIN_SPI_SS, OUTPUT);
    digitalWrite(PIN_SPI_SS, HIGH);

    card.init(SD_CS, 0);
    volume.init(card);
    rootDir.openRoot(volume);
    char name[] = "levels.bin";
    file.open(rootDir, name);
}

LevelLoader::~LevelLoader() {
    file.close();
    rootDir.close();
    SPI.end();
    pinMode(SD_CS, OUTPUT);
    digitalWrite(SD_CS, HIGH);
}

bool LevelLoader::loadNextLevel() {
    uint8_t currLevel = 0;
    file.read(reinterpret_cast<char*>(&currLevel), sizeof(uint8_t));
    uint8_t num_levels = 0;
    file.read(reinterpret_cast<char*>(&num_levels), sizeof(uint8_t));

    if (Level::mCurrentLevel >= num_levels) {
        return false;
    }

    uint32_t offsetToLevelData = 0;
    for (int i = 0; i < Level::mCurrentLevel; i++) {
        uint32_t size = 0;
        file.read(reinterpret_cast<char*>(&size), sizeof(uint32_t));
        offsetToLevelData += size;
    }

    //skip next level sizes and past levels
    uint32_t seek = sizeof(uint32_t) * (num_levels - Level::mCurrentLevel) + offsetToLevelData;
    file.seekSet(file.readPosition() + seek);

    file.read(reinterpret_cast<char*>(&Level::levelW), sizeof(uint8_t));
    file.read(reinterpret_cast<char*>(&Level::levelH), sizeof(uint8_t));

    Serial.print("w: "); Serial.print(Level::levelW); Serial.print(" h: "); Serial.println(Level::levelH);

    for (int i = 0; i < Level::levelH; i++) {
        for (int j = 0; j < Level::levelW; j++) {
            file.read(reinterpret_cast<char*>(&Level::level[i][j]), sizeof(uint8_t));
        }
    }

    uint16_t value;
    file.read(reinterpret_cast<char*>(&value), sizeof(uint16_t));
    Level::mStartCoords.x = static_cast<float>(value);
    file.read(reinterpret_cast<char*>(&value), sizeof(uint16_t));
    Level::mStartCoords.y = static_cast<float>(value);

    file.read(reinterpret_cast<char*>(&value), sizeof(uint16_t));
    Level::mEndCoords.x = static_cast<float>(value);
    file.read(reinterpret_cast<char*>(&value), sizeof(uint16_t));
    Level::mEndCoords.y = static_cast<float>(value);

    uint8_t count = 0;
    vec2 pos;
    file.read(reinterpret_cast<char*>(&count), sizeof(uint8_t));
    for (int i = 0; i < count; i++) {
        file.read(reinterpret_cast<char*>(&value), sizeof(uint16_t));
        pos.x = static_cast<float>(value);
        file.read(reinterpret_cast<char*>(&value), sizeof(uint16_t));
        pos.y = static_cast<float>(value);
        Level::mEnemies.push_back(Enemy(pos, EnemyConfig::Type::SLIME));
    }

    file.read(reinterpret_cast<char*>(&count), sizeof(uint8_t));
    for (int i = 0; i < count; i++) {
        file.read(reinterpret_cast<char*>(&value), sizeof(uint16_t));
        pos.x = static_cast<float>(value);
        file.read(reinterpret_cast<char*>(&value), sizeof(uint16_t));
        pos.y = static_cast<float>(value);
        Level::mEnemies.push_back(Enemy(pos, EnemyConfig::Type::BUG));
    }

    file.read(reinterpret_cast<char*>(&count), sizeof(uint8_t));
    for (int i = 0; i < count; i++) {
        file.read(reinterpret_cast<char*>(&value), sizeof(uint16_t));
        pos.x = static_cast<float>(value);
        file.read(reinterpret_cast<char*>(&value), sizeof(uint16_t));
        pos.y = static_cast<float>(value);
        Level::mPickups.push_back(Pickup(pos, 50, 0));
    }

    file.close();

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
