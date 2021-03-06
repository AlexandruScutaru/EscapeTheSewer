#include "level.h"
#include "player.h"
#include "color_palette.h"
#include "vec2.inl"

#if defined (ARDUINO) || defined (__AVR_ATmega328P__)
    #include "graphics.h"
    #define GET_TILE_ROW(tileIndex, rowIndex) (Level::tile_row_t { .packed = pgm_read_dword_near(&tiles[tileIndex][rowIndex]) })
    #define GET_TILE_FROM_LEVEL(i, j) (Level::tile_index_t { .packed = pgm_read_byte_near(&level[i][j]) })
#else
    #include "../pc_version/graphics_pc.h"

    #define min(a,b) ((a)<(b)?(a):(b))
    #define max(a,b) ((a)>(b)?(a):(b))

    #define GET_TILE_ROW(tileIndex, rowIndex) (tiles[tileIndex][rowIndex])
    #define GET_TILE_FROM_LEVEL(i, j) (level[i][j])
#endif

#define IF_NON_COLLIDABLE(tile) if (tile.tile_index.index >= TILE_NON_COLLIDABLE_THRESHOLD)

const uint16_t Level::colors[16] = {
    COLOR_BROWN_DARKER    ,COLOR_BROWN_DARK   ,COLOR_BROWN        ,COLOR_BROWN_LIGHT,
    COLOR_BROWN_LIGHTER   ,COLOR_BEIGE_DARK   ,COLOR_BEIGE        ,COLOR_GREEN_DARK,
    COLOR_GREEN_LIGHT     ,COLOR_MUSTARD      ,COLOR_BLUE_DARK    ,COLOR_CYAN,
    COLOR_RED             ,COLOR_ORANGE       ,COLOR_YELLOW       ,COLOR_WHITISH
};

vec2 Level::mStartCoords;
vec2 Level::mEndCoords;
vector<Enemy> Level::mEnemies;
Graphics* Level::mGraphics = nullptr;

//generated tiles array
#include "tiles_data.h"

//genrated level matrix
#include "level_data.h"

void Level::init() {
    populateSpecialObjects();
}

void Level::update(float dt) {
    for (size_t i = 0; i < mEnemies.size(); i++) {
        mEnemies[i].update(dt);
    }
}

void Level::cleanPrevDrawSpecialObjects() {
    for (size_t i = 0; i < mEnemies.size(); i++) {
        mEnemies[i].cleanPrevDraw(*mGraphics);
    }
}

void Level::drawSpecialObjects() {
    for (size_t i = 0; i < mEnemies.size(); i++) {
        mEnemies[i].draw(*mGraphics);
    }
}

void Level::drawEntireLevel() {
    for (int i = 0; i < levelH; i++) {
        for(int j = Graphics::camera.x1; j < Graphics::camera.x2; j++) {
            const auto& tile_index = getTileByIndex(i, j);
            mGraphics->drawTile(tile_index.tile_index.index,  j*TILE_SIZE, i*TILE_SIZE, TILE_SIZE, tile_index.tile_index.flip);
        }
    }
}

void Level::setGraphics(Graphics* graphics) {
    mGraphics = graphics;
}

uint8_t Level::getTileByPosition(uint16_t x, uint16_t y) {
    return  GET_TILE_FROM_LEVEL(y>>3, x>>3).tile_index.index;
}

Level::tile_index_t Level::getTileByIndex(uint8_t i, uint8_t j) {
    //don't want to add boundary checks here, as intances when it could be actually out of bounds are not that common
    //so saving overhead on ifs when not needed
    return  GET_TILE_FROM_LEVEL(i, j);
}

Level::tile_row_t Level::getTileRow(uint8_t tileIndex, uint8_t rowIndex) {
    return GET_TILE_ROW(tileIndex, rowIndex);
}

bool Level::collidesWithEnd(const vec2& pos) {
    return aabb(mEndCoords, pos);
}

bool Level::collideWithLevel(vec2& pos, const vec2& oldPos, vec2& velocity, const vec2& velocityToSet, bool* flip, bool* onGround, uint16_t* ladderXpos) {
    uint8_t leftUp    = Level::getTileByPosition(pos.x + 0.0f, oldPos.y + 0.0f);
    uint8_t leftDown  = Level::getTileByPosition(pos.x + 0.0f, oldPos.y + TILE_SIZE-1);
    uint8_t rightUp   = Level::getTileByPosition(pos.x + TILE_SIZE, oldPos.y + 0.0f);
    uint8_t rightDown = Level::getTileByPosition(pos.x + TILE_SIZE, oldPos.y + TILE_SIZE-1);
    if(velocity.x <= 0.0f) {
        if(leftUp < TILE_NON_COLLIDABLE_THRESHOLD || leftDown < TILE_NON_COLLIDABLE_THRESHOLD || pos.x <= 0.0f) {
            pos.x =  pos.x <= 0.0f ? 0.0f : (((uint16_t)pos.x >> 3) + 1) << 3;
            velocity.x = velocityToSet.x;
            if (flip)
                *flip = false;
        }
    } else {
        if(rightUp < TILE_NON_COLLIDABLE_THRESHOLD || rightDown < TILE_NON_COLLIDABLE_THRESHOLD ||  pos.x > levelW << 3) {
            pos.x = pos.x > levelW << 3 ? (levelW - 1) << 3 : ((uint16_t)pos.x >> 3) << 3;
            velocity.x = -velocityToSet.x;
            if (flip)
                *flip = true;
        }
    }

    if (onGround)
        *onGround = false;

    uint8_t upLeft    = Level::getTileByPosition(pos.x + 0.0f, pos.y);
    uint8_t upRight   = Level::getTileByPosition(pos.x + TILE_SIZE-1, pos.y);
    uint8_t downLeft  = Level::getTileByPosition(pos.x + 0.0f, pos.y + TILE_SIZE);
    uint8_t downRight = Level::getTileByPosition(pos.x + TILE_SIZE-1, pos.y + TILE_SIZE);
    if(velocity.y <= 0.0f) {
        if (upLeft < TILE_NON_COLLIDABLE_THRESHOLD || upRight < TILE_NON_COLLIDABLE_THRESHOLD) {
            pos.y = (((uint16_t)pos.y >> 3) + 1) << 3;
            velocity.y = velocityToSet.y;
        }
    } else {
        if (downLeft < TILE_NON_COLLIDABLE_THRESHOLD || downRight < TILE_NON_COLLIDABLE_THRESHOLD) {
            pos.y = min(((uint16_t)pos.y >> 3) << 3, 120);
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

bool Level::collideWithEnemies(Player& player) {
    for (size_t i = 0; i < mEnemies.size(); i++) {
        if (aabb(mEnemies[i].getPos(), player.getPos())) {
            if (!player.hit(mEnemies[i].getDmg())) {
                //player died
                return true;
            }
        }
    }
    return false;
}

void Level::cleanPrevDraw(const vec2& oldPos) {
    //clean up (redraw) the 4 adjacent tiles behind last drawn position
    //find maybe a way to draw when it is actually needed, as sending data to LCD to be displayed is very costly
    //TODO:
    //  do the checks based on velocity, maybe something can come out of that
    uint8_t iCoord = (uint16_t)oldPos.y >> 3;
    uint8_t jCoord = (uint16_t)oldPos.x >> 3;

    Level::tile_index_t tileIndex = Level::getTileByIndex(iCoord, jCoord);
    IF_NON_COLLIDABLE(tileIndex) mGraphics->drawTile(tileIndex.tile_index.index, jCoord << 3, iCoord << 3, TILE_SIZE, tileIndex.tile_index.flip);

    if (iCoord + 1 < Level::levelH) {
        tileIndex = Level::getTileByIndex(iCoord+1, jCoord);
        IF_NON_COLLIDABLE(tileIndex) mGraphics->drawTile(tileIndex.tile_index.index, jCoord << 3, (iCoord+1) << 3, TILE_SIZE, tileIndex.tile_index.flip);

        if(jCoord + 1 < Level::levelW) {
            tileIndex = Level::getTileByIndex(iCoord, jCoord + 1);
            IF_NON_COLLIDABLE(tileIndex) mGraphics->drawTile(tileIndex.tile_index.index, (jCoord+1) << 3, iCoord << 3, TILE_SIZE, tileIndex.tile_index.flip);
            //both above are true, directly draw diagonaly too
            tileIndex = Level::getTileByIndex(iCoord + 1, jCoord + 1);
            IF_NON_COLLIDABLE(tileIndex) mGraphics->drawTile(tileIndex.tile_index.index, (jCoord+1) << 3, (iCoord+1) << 3, TILE_SIZE, tileIndex.tile_index.flip);
        }
    }else if(jCoord + 1 < Level::levelW) {
        tileIndex = Level::getTileByIndex(iCoord, jCoord + 1);
        IF_NON_COLLIDABLE(tileIndex) mGraphics->drawTile(tileIndex.tile_index.index, (jCoord+1) << 3, iCoord << 3, TILE_SIZE, tileIndex.tile_index.flip);
    }
}

void Level::removeEnemy(size_t idx) {
    mEnemies[idx].cleanPrevDraw(*mGraphics);
    mEnemies.erase(idx);
}

bool Level::hitEnemy(const vec2& pos, float dmg, float force) {
    size_t i = 0;
    bool res = false;
    while (i < mEnemies.size()) {
        if (aabb(mEnemies[i].getPos(), pos)) {
            if (!mEnemies[i].hit(dmg, force)) {
                removeEnemy(i);
                res = true;
            } else {
                i++;
            }
        } else {
            i++;
        }
    }

    return res;
}

bool Level::aabb(const vec2& pos1, const vec2& pos2) {
    if (pos1.x < pos2.x + TILE_SIZE &&
        pos1.x + TILE_SIZE > pos2.x &&
        pos1.y < pos2.y + TILE_SIZE &&
        pos1.y + TILE_SIZE > pos2.y)
    {
        return true;
    }
    return false;
}
