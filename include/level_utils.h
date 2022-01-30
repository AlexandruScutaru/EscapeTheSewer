#ifndef LEVEL_UTILS_H
#define LEVEL_UTILS_H

#include "level.h"

class Player;

namespace LevelUtils {
    uint8_t getTileByPosition(Level& level, uint16_t x, uint16_t y);
    Level::tile_index_t getTileByIndex(Level& level, uint8_t i, uint8_t j);
    Level::tile_row_t getTileRow(Level& level, uint8_t tileIndex, uint8_t rowIndex);
    uint16_t getColor(Level& level, uint8_t index);

    bool collideWithEnd(Level& level, const vec2& pos);
    bool collideWithLevel(Level& level, vec2& pos, const vec2& oldPos, vec2& velocity, const vec2& velocityToSet, bool* flip = nullptr, bool* onGround = nullptr, uint16_t* ladderXpos = nullptr);
    void collideWithPickups(Level& level, Graphics& graphics, Player& player);
    bool collideWithEnemies(Level& level, Player& player);

    void removeEnemy(Level& level, Graphics& graphics, size_t idx);
    void removePickup(Level& level, Graphics& graphics, size_t idx);
    bool hitEnemy(Level& level, Graphics& graphics, const vec2& pos, float dmg, float force);

    void drawEntireLevel(Level& level, Graphics& graphics);
    void cleanPrevDraw(Level& level, Graphics& graphics, const vec2& oldPos);
}

#endif // LEVEL_UTILS_H
