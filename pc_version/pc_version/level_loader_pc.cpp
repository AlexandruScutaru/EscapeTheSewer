#include "level_loader_pc.h"
#include "../../include/level.h"
#include "../../include/vec2.inl"

#include <fstream>
#include <string>


bool LevelLoader::loadNextLevel() {
    std::ifstream ifs{ "../../resources/levels.bin", std::ios::binary };
    if (!ifs) {
        return false;
    }

    ifs.read(reinterpret_cast<char*>(&Level::mCurrentLevel), sizeof(uint8_t));
    uint8_t num_levels = 0;
    ifs.read(reinterpret_cast<char*>(&num_levels), sizeof(uint8_t));

    if (Level::mCurrentLevel >= num_levels) {
        return false;
    }

    uint32_t offsetToLevelData = 0;
    for (int i = 0; i < Level::mCurrentLevel; i++) {
        uint32_t size = 0;
        ifs.read(reinterpret_cast<char*>(&size), sizeof(uint32_t));
        offsetToLevelData += size;
    }

    //skip next level sizes and past levels
    auto seek = static_cast<int64_t>(sizeof(uint32_t)) * (num_levels - Level::mCurrentLevel) + offsetToLevelData;
    ifs.seekg(seek, std::ios_base::cur); //static analizer complained that the result may overflow before eventual casting to 8 bytes

    auto _ = ifs.tellg();

    ifs.read(reinterpret_cast<char*>(&Level::levelW), sizeof(uint8_t));
    ifs.read(reinterpret_cast<char*>(&Level::levelH), sizeof(uint8_t));

    for (int i = 0; i < Level::levelH; i++) {
        for (int j = 0; j < Level::levelW; j++) {
            ifs.read(reinterpret_cast<char*>(&Level::level[i][j]), sizeof(uint8_t));
        }
    }

    uint16_t value;
    ifs.read(reinterpret_cast<char*>(&value), sizeof(uint16_t));
    Level::mStartCoords.x = static_cast<float>(value);
    ifs.read(reinterpret_cast<char*>(&value), sizeof(uint16_t));
    Level::mStartCoords.y = static_cast<float>(value);

    ifs.read(reinterpret_cast<char*>(&value), sizeof(uint16_t));
    Level::mEndCoords.x = static_cast<float>(value);
    ifs.read(reinterpret_cast<char*>(&value), sizeof(uint16_t));
    Level::mEndCoords.y = static_cast<float>(value);

    uint8_t count = 0;
    vec2 pos;
    ifs.read(reinterpret_cast<char*>(&count), sizeof(uint8_t));
    for (int i = 0; i < count; i++) {
        ifs.read(reinterpret_cast<char*>(&value), sizeof(uint16_t));
        pos.x = static_cast<float>(value);
        ifs.read(reinterpret_cast<char*>(&value), sizeof(uint16_t));
        pos.y = static_cast<float>(value);
        //Level::mEnemies.push_back(Enemy(pos, EnemyConfig::GetConfig(EnemyConfig::Type::SLIME)));
    }

    ifs.read(reinterpret_cast<char*>(&count), sizeof(uint8_t));
    for (int i = 0; i < count; i++) {
        ifs.read(reinterpret_cast<char*>(&value), sizeof(uint16_t));
        pos.x = static_cast<float>(value);
        ifs.read(reinterpret_cast<char*>(&value), sizeof(uint16_t));
        pos.y = static_cast<float>(value);
        //Level::mEnemies.push_back(Enemy(pos, EnemyConfig::GetConfig(EnemyConfig::Type::BUG)));
    }

    ifs.read(reinterpret_cast<char*>(&count), sizeof(uint8_t));
    for (int i = 0; i < count; i++) {
        ifs.read(reinterpret_cast<char*>(&value), sizeof(uint16_t));
        pos.x = static_cast<float>(value);
        ifs.read(reinterpret_cast<char*>(&value), sizeof(uint16_t));
        pos.y = static_cast<float>(value);
        //Level::mPickups.push_back(Pickup(pos, 50, 0));
    }

    ifs.close();

    return true;
}

bool LevelLoader::writeCurrentLevelIndex() {
    std::ofstream ifs{ "../../resources/levels.bin", std::ios::binary | std::ios::in };
    if (!ifs) {
        return false;
    }

    ifs.seekp(0, std::ios_base::beg);
    ifs.write(reinterpret_cast<char*>(&Level::mCurrentLevel), sizeof(uint8_t));
    ifs.close();

    return true;
}
