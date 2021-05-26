#include "level.h"
#include "color_palette.h"
#include "vec2.inl"

#if defined (ARDUINO) || defined (__AVR_ATmega328P__)
    #include "graphics.h"
    #define GET_TILE_ROW(tileIndex, rowIndex) return Level::tile_row_t { .packed = pgm_read_dword_near(&tiles[tileIndex][rowIndex]) }
#else
    #include "../pc_version/graphics_pc.h"
    #define GET_TILE_ROW(tileIndex, rowIndex) return tiles[tileIndex][rowIndex]
#endif


const uint16_t Level::colors[16] = {
    COLOR_BROWN_DARKER    ,COLOR_BROWN_DARK   ,COLOR_BROWN        ,COLOR_BROWN_LIGHT,
    COLOR_BROWN_LIGHTER   ,COLOR_BEIGE_DARK   ,COLOR_BEIGE        ,COLOR_GREEN_DARK,
    COLOR_GREEN_LIGHT     ,COLOR_MUSTARD      ,COLOR_BLUE_DARK    ,COLOR_CYAN,
    COLOR_RED             ,COLOR_ORANGE       ,COLOR_YELLOW       ,COLOR_WHITISH
};

vec2 Level::mStartCoords;
vec2 Level::mEndCoords;
vector<Slime> Level::mSlimes;
vector<Coin> Level::mCoins;
Graphics* Level::mGraphics = nullptr;

//contains the tileset as arrays of tile_t
#include "tiles_data.h"

//for the future:
//      - implement a sparse matrix for the level, a lot of entries are just background colored/empty tiles
#include "level_data.h"


void Level::init() {
    populateSpecialObjects();
}

void Level::update(float dt) {
    for (size_t i = 0; i < mSlimes.size(); i++)
        mSlimes[i].update(dt);

    for (size_t i = 0; i < mCoins.size(); i++)
        mCoins[i].update(dt);
}

void Level::cleanPrevDraw() {
    for (size_t i = 0; i < mSlimes.size(); i++) {
        mSlimes[i].cleanPrevDraw(*mGraphics);
    }
}

void Level::draw() {
    for (size_t i = 0; i < mCoins.size(); i++) {
        mCoins[i].draw(*mGraphics);
    }

    for (size_t i = 0; i < mSlimes.size(); i++) {
        mSlimes[i].draw(*mGraphics);
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
    return level[y>>3][x>>3].tile_index.index;
}

Level::tile_index_t Level::getTileByIndex(uint8_t i, uint8_t j) {
    //don't want to add boundary checks here, as intances when it could be actually out of bounds are not that common
    //so saving overhead on ifs when not needed
    return level[i][j];
}

Level::tile_row_t Level::getTileRow(uint8_t tileIndex, uint8_t rowIndex) {
    GET_TILE_ROW(tileIndex, rowIndex);
}

Level::EntityType Level::getCollidedEntity(const vec2& pos, size_t& idx) {
    //pretty slow way to do it, I guess it is ok if not many entities are added to a level
    for (size_t i = 0; i < mSlimes.size(); i++) {
        if (aabb(mSlimes[i].getPos(), pos)) {
            idx = i;
            return EntityType::SLIME;
        }
    }
    for (size_t i = 0; i < mCoins.size(); i++) {
        if (aabb(mCoins[i].getPos(), pos)) {
            idx = i;
            return EntityType::COIN;
        }
    }
    if (aabb(mEndCoords, pos)) {
        return EntityType::END;
    }
    return EntityType::NONE;
}

void Level::removeEntity(EntityType entt, size_t idx) {
    switch (entt) {
    case EntityType::SLIME:
        mSlimes[idx].cleanPrevDraw(*mGraphics);
        mSlimes.erase(idx);
        break;
    case EntityType::COIN:
        mGraphics->drawFillRect(mCoins[idx].getPos().x, mCoins[idx].getPos().y, TILE_SIZE, TILE_SIZE);
        mCoins.erase(idx);
        break;
    default:
        //no op
        break;
    }
}

void Level::hitEntity(EntityType entt, size_t idx, float dmg, float force) {
    switch (entt) {
    case EntityType::SLIME:
        if (!mSlimes[idx].hit(dmg, force)) {
            removeEntity(EntityType::SLIME, idx);
        }
        break;
    default:
        break;
    }
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
