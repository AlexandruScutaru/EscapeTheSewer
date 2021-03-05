#include "data.h"
#include "color_palette.h"

#if defined (ARDUINO) || defined (__AVR_ATmega328P__)
    #define GET_TILE_ROW(tileIndex, rowIndex) return Data::tile_row_t { .packed = pgm_read_dword_near(&tiles[tileIndex][rowIndex]) }
#else
    #define GET_TILE_ROW(tileIndex, rowIndex) return tiles[tileIndex][rowIndex]
#endif

const uint16_t Data::colors[16] = {
    COLOR_BROWN_DARKER  , COLOR_BROWN_DARK   ,COLOR_BROWN        ,COLOR_BROWN_LIGHT,
    COLOR_BROWN_LIGHTER , COLOR_BEIGE_DARK   ,COLOR_BEIGE        ,COLOR_GREEN_DARK,
    COLOR_GREEN_LIGHT   , COLOR_MUSTARD      ,COLOR_BLUE_DARK    ,COLOR_CYAN,
    COLOR_RED           , COLOR_ORANGE       ,COLOR_YELLOW       ,COLOR_WHITISH
};

//contains the tileset as arrays of tile_t
#include "tiles.h"

//define a list of meta-tiles of tiles from the list above, if there is a reason to
/*
const metatile_t metatiles[] PROGMEM = {
    {
        {{.index = 1, .flip = 0}},
        {{.index = 2, .flip = 0}},
        {{.index = 3, .flip = 0}},
        {{.index = 0, .flip = 0}},
    }
};
*/

//for the future:
//      - implement a sparse matrix for the level, a lot of entries are just background colored/empty tiles
//      - don't store it in progmem but actually make it loadable from sdcard or if the game is suited make the levels procedural
//      - agree on a level size, 16x20 for now
#include "level.h"


uint8_t Data::getTileByPosition(uint16_t x, uint16_t y) {
    return level[y>>3][x>>3].tile_index.index;
}

Data::tile_index_t Data::getTileByIndex(uint8_t i, uint8_t j) {
    return level[i][j];
}

Data::tile_row_t Data::getTileRow(uint8_t tileIndex, uint8_t rowIndex) {
    GET_TILE_ROW(tileIndex, rowIndex);
}