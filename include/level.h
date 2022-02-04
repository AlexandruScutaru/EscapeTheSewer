#ifndef LEVEL_H
#define LEVEL_H

#include "array.h"
#include "enemy.h"
#include "pickup.h"
#include "vec2.h"

#if defined (ARDUINO) || defined (__AVR_ATmega328P__)
    #include <Arduino.h>
#else
    #define PROGMEM
#endif

#include <stdint.h>

#define TILE_SIZE          8
#define PALETTE_COUNT     16

#define TILE_LADDER                   19
#define TILE_EMPTY                    48
#define TILE_NON_COLLIDABLE_THRESHOLD 19

#define LEVEL_MAX_W 50
#define LEVEL_MAX_H 15

#define ENEMIES_PER_LEVEL 4
#define PICKUPS_PER_LEVEL 3


struct Level {
    union tile_row_t {
        struct row_s{
            uint32_t col1 : 4;
            uint32_t col2 : 4;
            uint32_t col3 : 4;
            uint32_t col4 : 4;
            uint32_t col5 : 4;
            uint32_t col6 : 4;
            uint32_t col7 : 4;
            uint32_t col8 : 4;

            uint8_t operator[] (size_t idx) const {
                switch (idx) {
                    case 1: return col1;
                    case 2: return col2;
                    case 3: return col3;
                    case 4: return col4;
                    case 5: return col5;
                    case 6: return col6;
                    case 7: return col7;
                    case 8: return col8;
                    default: return 0;
                }
            }
        } row;
        uint32_t packed;
    };

    typedef tile_row_t tile_t[8];

    union tile_index_t {
        struct tile_index_s {
            uint8_t index : 6;
            uint8_t flip  : 2;
        } tile_index;
        uint8_t packed;
    };

    tile_index_t levelData[LEVEL_MAX_H][LEVEL_MAX_W] = {};
    const static uint16_t colors[PALETTE_COUNT] /*PROGMEM*/;
    const static tile_t tiles[] PROGMEM;

    //I haven't tried making these polymorphic, not sure if I want to add vtable overhead on arduino
    array<Enemy, ENEMIES_PER_LEVEL> enemies;
    array<Pickup, PICKUPS_PER_LEVEL> pickups;
    vec2 startCoords;
    vec2 endCoords;
    uint8_t levelW = 0;
    uint8_t levelH = 0;

};

#endif // LEVEL_H
