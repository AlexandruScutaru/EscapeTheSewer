#ifndef DATA_H
#define DATA_H

#if defined (ARDUINO) || defined (__AVR_ATmega328P__)
    #include <Arduino.h>
#else
    #define PROGMEM
#endif

#include <stdint.h>

#define TILE_SIZE  8

#define ANIM_IDLE_START    4
#define ANIM_WALK_START    0
#define ANIM_JUMP_START   10
#define ANIM_FALL_START    8
#define ANIM_ATTACK_START 14

#define ANIM_IDLE_FRAMES   4
#define ANIM_WALK_FRAMES   4
#define ANIM_JUMP_FRAMES   2
#define ANIM_FALL_FRAMES   2
#define ANIM_ATTACK_FRAMES 2


class Data {
public:
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

    struct metatile_t {
        tile_index_t ru;
        tile_index_t rd;
        tile_index_t ld;
        tile_index_t lu;
    };

    static uint8_t getTileByPosition(uint16_t x, uint16_t y);
    static tile_index_t getTileByIndex(uint8_t i, uint8_t j);
    static tile_row_t getTileRow(uint8_t tileIndex, uint8_t rowIndex);

    const static uint8_t levelW = 20;
    const static uint8_t levelH = 16;
    const static uint16_t colors[16] /*PROGMEM*/;

private:
    const static tile_index_t level[levelH][levelW] /*PROGMEM*/;
    const static tile_t tiles[] PROGMEM;

};

#endif // DATA_H