#include "level_loader_pc.h"
#include "../../include/level.h"
#include "../../include/vec2.inl"

#include <fstream>
#include <string>


Level LevelLoader::loadLevel(uint8_t index) {
    std::ifstream ifs{ "../../resources/levels.bin", std::ios::binary };
    if (!ifs) {
        return {};
    }

    uint8_t currentLevel = 0;
    ifs.read(reinterpret_cast<char*>(&currentLevel), sizeof(uint8_t));
    uint8_t num_levels = 0;
    ifs.read(reinterpret_cast<char*>(&num_levels), sizeof(uint8_t));

    if (index >= num_levels) {
        return {};
    }

    uint32_t offsetToLevelData = 0;
    for (int i = 0; i < index; i++) {
        uint32_t size = 0;
        ifs.read(reinterpret_cast<char*>(&size), sizeof(uint32_t));
        offsetToLevelData += size;
    }

    //skip next level sizes and past levels
    auto seek = static_cast<int64_t>(sizeof(uint32_t)) * (num_levels - index) + offsetToLevelData;
    ifs.seekg(seek, std::ios_base::cur); //static analizer complained that the result may overflow before eventual casting to 8 bytes

    auto _ = ifs.tellg();

    Level level;

    ifs.read(reinterpret_cast<char*>(&level.levelW), sizeof(uint8_t));
    ifs.read(reinterpret_cast<char*>(&level.levelH), sizeof(uint8_t));

    for (int i = 0; i < level.levelH; i++) {
        for (int j = 0; j < level.levelW; j++) {
            ifs.read(reinterpret_cast<char*>(&level.levelData[i][j]), sizeof(uint8_t));
        }
    }

    uint16_t value;
    ifs.read(reinterpret_cast<char*>(&value), sizeof(uint16_t));
    level.startCoords.x = static_cast<float>(value);
    ifs.read(reinterpret_cast<char*>(&value), sizeof(uint16_t));
    level.startCoords.y = static_cast<float>(value);

    ifs.read(reinterpret_cast<char*>(&value), sizeof(uint16_t));
    level.endCoords.x = static_cast<float>(value);
    ifs.read(reinterpret_cast<char*>(&value), sizeof(uint16_t));
    level.endCoords.y = static_cast<float>(value);

    uint8_t count = 0;
    vec2 pos;
    ifs.read(reinterpret_cast<char*>(&count), sizeof(uint8_t));
    for (int i = 0; i < count; i++) {
        ifs.read(reinterpret_cast<char*>(&value), sizeof(uint16_t));
        pos.x = static_cast<float>(value);
        ifs.read(reinterpret_cast<char*>(&value), sizeof(uint16_t));
        pos.y = static_cast<float>(value);
        level.enemies.push_back(Enemy(pos, EnemyConfig::Type::SLIME));
    }

    ifs.read(reinterpret_cast<char*>(&count), sizeof(uint8_t));
    for (int i = 0; i < count; i++) {
        ifs.read(reinterpret_cast<char*>(&value), sizeof(uint16_t));
        pos.x = static_cast<float>(value);
        ifs.read(reinterpret_cast<char*>(&value), sizeof(uint16_t));
        pos.y = static_cast<float>(value);
        level.enemies.push_back(Enemy(pos, EnemyConfig::Type::BUG));
    }

    ifs.read(reinterpret_cast<char*>(&count), sizeof(uint8_t));
    for (int i = 0; i < count; i++) {
        ifs.read(reinterpret_cast<char*>(&value), sizeof(uint16_t));
        pos.x = static_cast<float>(value);
        ifs.read(reinterpret_cast<char*>(&value), sizeof(uint16_t));
        pos.y = static_cast<float>(value);
        level.pickups.push_back(Pickup(pos, 50, 0));
    }

    ifs.close();

    return level;
}

bool LevelLoader::writeCurrentLevelIndex() {
    //std::ofstream ifs{ "../../resources/levels.bin", std::ios::binary | std::ios::in };
    //if (!ifs) {
    //    return false;
    //}
    //
    //ifs.seekp(0, std::ios_base::beg);
    //ifs.write(reinterpret_cast<char*>(&Level::mCurrentLevel), sizeof(uint8_t));
    //ifs.close();

    return true;
}
