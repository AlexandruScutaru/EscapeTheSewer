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
vector<vec2> Level::mLadders;
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

void Level::draw() {
    for (size_t i = 0; i < mSlimes.size(); i++)
        mSlimes[i].draw(*mGraphics);

    for (size_t i = 0; i < mCoins.size(); i++)
        mCoins[i].draw(*mGraphics);
}

void Level::drawEntireLevel() {
    for (int i = 0; i < levelH; i++) {
        for(int j = 0; j < levelW; j++) {
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
    return level[i][j];
}

Level::tile_row_t Level::getTileRow(uint8_t tileIndex, uint8_t rowIndex) {
    GET_TILE_ROW(tileIndex, rowIndex);
}