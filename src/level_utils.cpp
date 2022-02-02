#include "level_utils.h"
#include "player.h"


#if defined (ARDUINO) || defined (__AVR_ATmega328P__)
    #include "graphics.h"

    #define GET_TILE_ROW(level, tileIndex, rowIndex) (Level::tile_row_t { .packed = pgm_read_dword_near(&level.tiles[tileIndex][rowIndex]) })
    #define GET_COLOR(level, index) (pgm_read_dword_near(&level.colors[index]))
#else
    #include "../pc_version/pc_version/graphics_pc.h"

    #define min(a,b) ((a)<(b)?(a):(b))
    #define max(a,b) ((a)>(b)?(a):(b))

    #define GET_TILE_ROW(level, tileIndex, rowIndex) (level.tiles[tileIndex][rowIndex])
    #define GET_COLOR(level, index) (level.colors[index])
#endif

#define IF_NON_COLLIDABLE(tile) if (tile.tile_index.index >= TILE_NON_COLLIDABLE_THRESHOLD)

bool aabb(const vec2& pos1, const vec2& pos2) {
    if (pos1.x < pos2.x + TILE_SIZE &&
        pos1.x + TILE_SIZE > pos2.x &&
        pos1.y < pos2.y + TILE_SIZE &&
        pos1.y + TILE_SIZE > pos2.y)
    {
        return true;
    }
    return false;
}


uint8_t LevelUtils::getTileByPosition(Level& level, uint16_t x, uint16_t y) {
    return  level.levelData[y>>3][x>>3].tile_index.index;
}

Level::tile_index_t LevelUtils::getTileByIndex(Level& level, uint8_t i, uint8_t j) {
    //don't want to add boundary checks here, as intances when it could be actually out of bounds are not that common
    //so saving overhead on ifs when not needed
    return  level.levelData[i][j];
}

Level::tile_row_t LevelUtils::getTileRow(Level& level, uint8_t tileIndex, uint8_t rowIndex) {
    return GET_TILE_ROW(level, tileIndex, rowIndex);
}

uint16_t LevelUtils::getColor(Level& level, uint8_t index) {
    return GET_COLOR(level, index);
}


bool LevelUtils::collideWithEnd(Level& level, const vec2& pos) {
    if (aabb(level.endCoords, pos)) {
        return true;
    }

    return false;
}

bool LevelUtils::collideWithLevel(Level& level, vec2& pos, const vec2& oldPos, vec2& velocity, const vec2& velocityToSet, bool* flip, bool* onGround, uint16_t* ladderXpos) {
    uint8_t leftUp    = getTileByPosition(level, static_cast<uint16_t>(pos.x + 0.0f), static_cast<uint16_t>(oldPos.y + 0.0f));
    uint8_t leftDown  = getTileByPosition(level, static_cast<uint16_t>(pos.x + 0.0f), static_cast<uint16_t>(oldPos.y + TILE_SIZE-1));
    uint8_t rightUp   = getTileByPosition(level, static_cast<uint16_t>(pos.x + TILE_SIZE), static_cast<uint16_t>(oldPos.y + 0.0f));
    uint8_t rightDown = getTileByPosition(level, static_cast<uint16_t>(pos.x + TILE_SIZE), static_cast<uint16_t>(oldPos.y + TILE_SIZE-1));
    if(velocity.x <= 0.0f) {
        if(leftUp < TILE_NON_COLLIDABLE_THRESHOLD || leftDown < TILE_NON_COLLIDABLE_THRESHOLD || pos.x <= 0.0f) {
            pos.x =  pos.x <= 0.0f ? 0.0f : static_cast<float>((((uint16_t)pos.x >> 3) + 1) << 3);
            velocity.x = velocityToSet.x;
            if (flip)
                *flip = false;
        }
    } else {
        if(rightUp < TILE_NON_COLLIDABLE_THRESHOLD || rightDown < TILE_NON_COLLIDABLE_THRESHOLD ||  pos.x > level.levelW << 3) {
            pos.x = pos.x > level.levelW << 3 ? static_cast<float>((level.levelW - 1) << 3) : static_cast<float>(((uint16_t)pos.x >> 3) << 3);
            velocity.x = -velocityToSet.x;
            if (flip)
                *flip = true;
        }
    }

    if (onGround)
        *onGround = false;

    uint8_t upLeft    = getTileByPosition(level, static_cast<uint16_t>(pos.x + 0.0f), static_cast<uint16_t>(pos.y));
    uint8_t upRight   = getTileByPosition(level, static_cast<uint16_t>(pos.x + TILE_SIZE-1), static_cast<uint16_t>(pos.y));
    uint8_t downLeft  = getTileByPosition(level, static_cast<uint16_t>(pos.x + 0.0f), static_cast<uint16_t>(pos.y + TILE_SIZE));
    uint8_t downRight = getTileByPosition(level, static_cast<uint16_t>(pos.x + TILE_SIZE-1), static_cast<uint16_t>(pos.y + TILE_SIZE));
    if(velocity.y <= 0.0f) {
        if (upLeft < TILE_NON_COLLIDABLE_THRESHOLD || upRight < TILE_NON_COLLIDABLE_THRESHOLD) {
            pos.y = static_cast<float>((((uint16_t)pos.y >> 3) + 1) << 3);
            velocity.y = velocityToSet.y;
        }
    } else {
        if (downLeft < TILE_NON_COLLIDABLE_THRESHOLD || downRight < TILE_NON_COLLIDABLE_THRESHOLD) {
            pos.y = static_cast<float>(min(((uint16_t)pos.y >> 3) << 3, 120));
            velocity.y = velocityToSet.y;
            if (onGround)
                *onGround = true;
        }
    }

    if (ladderXpos) {
        bool collidedWithLadder = ( leftUp    == TILE_LADDER ||
                                    leftDown  == TILE_LADDER ||
                                    rightUp   == TILE_LADDER ||
                                    rightDown == TILE_LADDER ||
                                    upLeft    == TILE_LADDER ||
                                    upRight   == TILE_LADDER ||
                                    downLeft  == TILE_LADDER ||
                                    downRight == TILE_LADDER );
        if (collidedWithLadder) {
            if (leftUp == TILE_LADDER || leftDown == TILE_LADDER || upLeft == TILE_LADDER || downLeft == TILE_LADDER)
                *ladderXpos = (((uint16_t)pos.x >> 3) + 0) << 3;
            else
                *ladderXpos = (((uint16_t)pos.x >> 3) + 1) << 3;
            return true;
        }
    }

    return false;
}

void LevelUtils::collideWithPickups(Level& level, Graphics& graphics, Player& player) {
    //for (uint8_t i = 0; i < level.pickups.size(); i++) {
    //   if (aabb(level.pickups[i].getPos(), player.getPos())) {
    //       player.incHp(30); //hardcoded way of doing it, to be updated
    //       removePickup(level, graphics, i);
    //   }
    //}
}

bool LevelUtils::collideWithEnemies(Level& level, Player& player) {
    //for (uint8_t i = 0; i < level.enemies.size(); i++) {
    //   if (aabb(level.enemies[i].getPos(), player.getPos())) {
    //       if (!player.hit(level.enemies[i].getDmg())) {
    //           //player died
    //           return true;
    //       }
    //   }
    //}
    return false;
}

void LevelUtils::removeEnemy(Level& level, Graphics& graphics, uint8_t idx) {
    //level.enemies[idx].cleanPrevDraw(level, graphics);
    //level.enemies.erase(idx);
}

void LevelUtils::removePickup(Level& level, Graphics& graphics, uint8_t idx) {
    //level.pickups[idx].cleanPrevDraw(level, graphics);
    //level.pickups.erase(idx);
}

bool LevelUtils::hitEnemy(Level& level, Graphics& graphics, const vec2& pos, float dmg, float force) {
    uint8_t i = 0;
    bool res = false;
    //while (i < level.enemies.size()) {
    //   if (aabb(level.enemies[i].getPos(), pos)) {
    //       if (!level.enemies[i].hit(static_cast<int8_t>(dmg), static_cast<int8_t>(force))) {
    //           removeEnemy(level, graphics, i);
    //           res = true;
    //       } else {
    //           i++;
    //       }
    //   } else {
    //       i++;
    //   }
    //}

    return res;
}

void LevelUtils::drawEntireLevel(Level& level, Graphics& graphics) {
    for (int i = 0; i < level.levelH; i++) {
        for(int j = Graphics::camera.x1; j < Graphics::camera.x2; j++) {
            const auto& tile_index = getTileByIndex(level, i, j);
            graphics.drawTile(tile_index.tile_index.index,  j*TILE_SIZE, i*TILE_SIZE, TILE_SIZE, tile_index.tile_index.flip);
        }
    }
}

void LevelUtils::cleanPrevDraw(Level& level, Graphics& graphics,const vec2& oldPos) {
    //clean up (redraw) the 4 adjacent tiles behind last drawn position
    //find maybe a way to draw when it is actually needed, as sending data to LCD to be displayed is very costly
    //TODO:
    //  do the checks based on velocity, maybe something can come out of that
    uint8_t iCoord = (uint16_t)oldPos.y >> 3;
    uint8_t jCoord = (uint16_t)oldPos.x >> 3;

    Level::tile_index_t tileIndex = getTileByIndex(level, iCoord, jCoord);
    IF_NON_COLLIDABLE(tileIndex) graphics.drawTile(tileIndex.tile_index.index, jCoord << 3, iCoord << 3, TILE_SIZE, tileIndex.tile_index.flip);

    if (iCoord + 1 < level.levelH) {
        tileIndex = getTileByIndex(level, iCoord+1, jCoord);
        IF_NON_COLLIDABLE(tileIndex) graphics.drawTile(tileIndex.tile_index.index, jCoord << 3, (iCoord+1) << 3, TILE_SIZE, tileIndex.tile_index.flip);

        if(jCoord + 1 < level.levelW) {
            tileIndex = getTileByIndex(level, iCoord, jCoord + 1);
            IF_NON_COLLIDABLE(tileIndex) graphics.drawTile(tileIndex.tile_index.index, (jCoord+1) << 3, iCoord << 3, TILE_SIZE, tileIndex.tile_index.flip);
            //both above are true, directly draw diagonaly too
            tileIndex = getTileByIndex(level, iCoord + 1, jCoord + 1);
            IF_NON_COLLIDABLE(tileIndex) graphics.drawTile(tileIndex.tile_index.index, (jCoord+1) << 3, (iCoord+1) << 3, TILE_SIZE, tileIndex.tile_index.flip);
        }
    }else if(jCoord + 1 < level.levelW) {
        tileIndex = getTileByIndex(level, iCoord, jCoord + 1);
        IF_NON_COLLIDABLE(tileIndex) graphics.drawTile(tileIndex.tile_index.index, (jCoord+1) << 3, iCoord << 3, TILE_SIZE, tileIndex.tile_index.flip);
    }
}
