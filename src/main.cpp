#include <Arduino.h>
#include <SPI.h>
#include <TFT_ST7735.h>

#include <stdint.h>

#include "input_manager.h"

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

//define array of limited number of colors -> start with 16 -> 4bit index
/*      name                        value    index */
#define COLOR_BROWN_DARKER          0x1061   /*  0 */               
#define COLOR_GREEN_LIGHT           0x32A3   /*  1 */               
#define COLOR_MUSTARD               0xA444   /*  2 */               
#define COLOR_YELLOW                0xED44   /*  3 */               
#define COLOR_WHITISH               0xEEB3   /*  4 */               
#define COLOR_BLUE_DARK             0x11E6   /*  5 */               
#define COLOR_ORANGE                0xFAE5   /*  6 */               
#define COLOR_BEIGE                 0xEDAE   /*  7 */               
#define COLOR_BEIGE_DARK            0xD4EC   /*  8 */               
#define COLOR_GREEN_DARK            0x1901   /*  9 */               
#define COLOR_BROWN_DARK            0x28E1   /* 10 */               
#define COLOR_BROWN                 0x3143   /* 11 */               
#define COLOR_BROWN_LIGHT           0x6247   /* 12 */               
#define COLOR_BROWN_LIGHTER         0x8BEC   /* 13 */               
#define COLOR_CYAN                  0x3CF2   /* 14 */               
#define COLOR_RED                   0x90C1   /* 15 */               

const uint16_t colors[16] PROGMEM = {COLOR_BROWN_DARKER,COLOR_GREEN_LIGHT,COLOR_MUSTARD,COLOR_YELLOW,COLOR_WHITISH,COLOR_BLUE_DARK,COLOR_ORANGE,COLOR_BEIGE,COLOR_BEIGE_DARK,COLOR_GREEN_DARK,COLOR_BROWN_DARK,COLOR_BROWN,COLOR_BROWN_LIGHT,COLOR_BROWN_LIGHTER,COLOR_CYAN,COLOR_RED};

//define a list of 8x8 tiles of colors from the above array -> a tile will have a size of 32 bytes -> maybe 64 would be enough -> 2048 bytes
const tile_t tiles[] PROGMEM = {
    {//walk1
        { 0, 0, 4, 4, 4, 4, 0, 0},
        { 0, 4, 8, 8, 8, 8, 4, 0},
        { 0, 4, 7, 7, 0,15, 4, 0},
        { 0, 0, 7, 7, 7,15, 0, 0},
        { 0, 0, 1, 1, 1, 0, 0, 0},
        { 0, 0, 2, 2, 2, 0, 0, 0},
        { 0, 8, 2, 2, 2, 0, 0, 0},
        { 0, 0, 8, 8, 0, 0, 0, 0},
    },
    {//walk2
        { 0, 0, 4, 4, 4, 4, 4, 0},
        { 0, 4, 8, 8, 8, 8, 0, 4},
        { 0, 4, 7, 7, 0,15, 0, 0},
        { 0, 0, 7, 7, 7,15, 0, 0},
        { 0, 0, 1, 1, 1, 0, 0, 0},
        { 0, 0, 2, 2, 2, 7, 0, 0},
        { 0, 0, 2, 2, 2, 7, 0, 0},
        { 0, 0, 7, 7, 0, 0, 0, 0},
    },
    {//walk3
        { 0, 0, 0, 0, 0, 0, 0, 0},
        { 0, 0, 4, 4, 4, 4, 4, 4},
        { 0, 4, 8, 8, 8, 8, 0, 0},
        { 0, 4, 7, 7, 0,15, 0, 0},
        { 0, 4, 7, 7, 7,15, 0, 0},
        { 0, 0, 1, 1, 1, 0, 0, 0},
        { 0, 2, 2, 2, 2, 2, 0, 0},
        { 7, 7, 0, 0, 0, 0, 7, 7},
    },
    {//walk4
        { 0, 0, 0, 0, 0, 0, 0, 0},
        { 0, 0, 4, 4, 4, 4, 4, 0},
        { 0, 4, 8, 8, 8, 8, 0, 4},
        { 0, 4, 7, 7, 0,15, 0, 0},
        { 0, 4, 7, 7, 7,15, 0, 0},
        { 0, 0, 1, 1, 1, 0, 0, 0},
        { 7, 2, 2, 2, 2, 0, 0, 0},
        { 7, 0, 0, 7, 7, 0, 0, 0},
    },
    {//idle1
        { 0, 0, 4, 4, 4, 4, 4, 0},
        { 0, 4, 8, 8, 8, 8, 0, 4},
        { 0, 4, 7, 7, 0,15, 0, 0},
        { 0, 0, 7, 7, 7,15, 0, 0},
        { 0, 0, 1, 1, 1, 0, 0, 0},
        { 0, 0, 2, 2, 2, 0, 0, 0},
        { 0, 0, 2, 2, 2, 0, 0, 0},
        { 0, 7, 7, 0, 7, 7, 0, 0},
    },
    {//idle2
        { 0, 0, 0, 0, 0, 0, 0, 0},
        { 0, 0, 4, 4, 4, 4, 4, 4},
        { 0, 4, 8, 8, 8, 8, 0, 0},
        { 0, 4, 7, 7, 0,15, 0, 0},
        { 0, 0, 7, 7, 7,15, 0, 0},
        { 0, 0, 1, 1, 1, 0, 0, 0},
        { 0, 2, 2, 2, 2, 2, 0, 0},
        { 0, 7, 7, 0, 7, 7, 0, 0},
    },
    {//idle3
        { 0, 0, 0, 0, 0, 0, 0, 0},
        { 0, 0, 4, 4, 4, 4, 4, 0},
        { 0, 4, 8, 8, 8, 8, 0, 4},
        { 0, 4, 7, 7, 0,15, 0, 0},
        { 0, 0, 7, 7, 7,15, 0, 0},
        { 0, 0, 1, 1, 1, 0, 0, 0},
        { 0, 2, 2, 2, 2, 2, 0, 0},
        { 0, 7, 7, 0, 7, 7, 0, 0},
    },
    {//idle4
        { 0, 0, 4, 4, 4, 4, 0, 0},
        { 0, 4, 8, 8, 8, 8, 4, 0},
        { 0, 4, 7, 7, 0,15, 4, 0},
        { 0, 0, 7, 7, 7,15, 0, 0},
        { 0, 0, 1, 1, 1, 0, 0, 0},
        { 0, 0, 2, 2, 2, 0, 0, 0},
        { 0, 0, 2, 2, 2, 0, 0, 0},
        { 0, 7, 7, 0, 7, 7, 0, 0},
    },
    {//fall1
        { 0, 0, 0, 0, 0, 0, 0, 4},
        { 0, 0, 4, 4, 4, 4, 4, 0},
        { 0, 4, 8, 8, 8, 8, 0, 0},
        { 0, 4, 7, 7, 0,15, 0, 0},
        { 0, 0, 7, 7, 7,15, 0, 0},
        { 0, 0, 1, 1, 1, 0, 0, 0},
        { 7, 0, 2, 2, 2, 0, 7, 0},
        { 0, 7, 2, 2, 2, 7, 0, 0},
    },
    {//fall2
        { 0, 0, 0, 0, 0, 0, 0, 0},
        { 0, 0, 4, 4, 4, 4, 4, 4},
        { 0, 4, 8, 8, 8, 8, 0, 0},
        { 0, 4, 7, 7, 0,15, 0, 0},
        { 0, 0, 7, 7, 7,15, 0, 0},
        { 0, 0, 1, 1, 1, 0, 0, 0},
        { 7, 0, 2, 2, 2, 0, 7, 0},
        { 0, 7, 2, 2, 2, 7, 0, 0},
    },
    {//jump1
        { 0, 0, 4, 4, 4, 4, 0, 0},
        { 0, 4, 8, 8, 8, 8, 4, 0},
        { 0, 4, 7, 7, 0,15, 4, 0},
        { 0, 0, 7, 7, 7,15, 0, 0},
        { 0, 0, 1, 1, 1, 0, 0, 0},
        { 0, 0, 2, 2, 2, 0, 0, 0},
        { 0, 0, 7, 0, 7, 0, 0, 0},
        { 0, 0, 7, 0, 7, 0, 0, 0},
    },    
    {//jump2
        { 0, 0, 4, 4, 4, 4, 0, 0},
        { 0, 4, 8, 8, 8, 8, 4, 0},
        { 0, 4, 7, 7, 0,15, 0, 4},
        { 0, 0, 7, 7, 7,15, 0, 0},
        { 0, 0, 1, 1, 1, 0, 0, 0},
        { 0, 0, 2, 2, 2, 0, 0, 0},
        { 0, 0, 7, 0, 7, 0, 0, 0},
        { 0, 0, 7, 0, 7, 0, 0, 0},
    },
    {//climb1
        { 0, 0, 4, 4, 4, 0, 0, 0},
        { 0, 4, 4, 4, 4, 4, 0, 0},
        { 0, 4, 4, 4, 4, 4, 0, 0},
        { 0, 4, 4, 4, 4, 4, 0, 0},
        { 0, 0, 1, 1, 1, 0, 0, 0},
        { 0, 0, 2, 2, 2, 2, 0, 0},
        { 0, 2, 2, 0, 7, 7, 0, 0},
        { 0, 7, 7, 0, 0, 0, 0, 0},
    },
    {//climb2
        { 0, 0, 0, 4, 4, 4, 0, 0},
        { 0, 0, 4, 4, 4, 4, 4, 0},
        { 0, 0, 4, 4, 4, 4, 4, 0},
        { 0, 0, 4, 4, 4, 4, 4, 0},
        { 0, 0, 0, 1, 1, 1, 0, 0},
        { 0, 0, 2, 2, 2, 2, 0, 0},
        { 0, 0, 7, 7, 0, 2, 2, 0},
        { 0, 0, 0, 0, 0, 7, 7, 0},
    },
    {//coin1                                                                                        14
        { 0, 0, 0, 0, 0, 0, 0, 0},
        { 0, 0, 0, 3, 6, 0, 0, 0},
        { 0, 0, 3, 7, 7, 6, 0, 0},
        { 0, 0, 3, 7, 7, 6, 0, 0},
        { 0, 0, 3, 7, 7, 6, 0, 0},
        { 0, 0, 0, 3, 6, 0, 0, 0},
        { 0, 0, 0, 0, 0, 0, 0, 0},
        { 0, 0, 0, 0, 0, 0, 0, 0},
    },
    {//coin2
        { 0, 0, 0, 0, 0, 0, 0, 0},
        { 0, 0, 0, 3, 6, 0, 0, 0},
        { 0, 0, 0, 3, 6, 0, 0, 0},
        { 0, 0, 0, 3, 6, 0, 0, 0},
        { 0, 0, 0, 3, 6, 0, 0, 0},
        { 0, 0, 0, 3, 6, 0, 0, 0},
        { 0, 0, 0, 0, 0, 0, 0, 0},
        { 0, 0, 0, 0, 0, 0, 0, 0},
    },
    {//coin3
        { 0, 0, 0, 0, 0, 0, 0, 0},
        { 0, 0, 0, 6, 3, 0, 0, 0},
        { 0, 0, 0, 6, 3, 0, 0, 0},
        { 0, 0, 0, 6, 3, 0, 0, 0},
        { 0, 0, 0, 6, 3, 0, 0, 0},
        { 0, 0, 0, 6, 3, 0, 0, 0},
        { 0, 0, 0, 0, 0, 0, 0, 0},
        { 0, 0, 0, 0, 0, 0, 0, 0},
    },
    {//coin4
        { 0, 0, 0, 0, 0, 0, 0, 0},
        { 0, 0, 0, 6, 3, 0, 0, 0},
        { 0, 0, 6, 7, 7, 3, 0, 0},
        { 0, 0, 6, 7, 7, 3, 0, 0},
        { 0, 0, 6, 7, 7, 3, 0, 0},
        { 0, 0, 0, 6, 3, 0, 0, 0},
        { 0, 0, 0, 0, 0, 0, 0, 0},
        { 0, 0, 0, 0, 0, 0, 0, 0},
    },
    {//platform1_1                                                                                  18
        {13,13,11,13,13,11,13,13},
        {13,13,12,12,13,12,13,12},
        {12,12,12,12,11,11,12,11},
        {12,11,11,10,11,11,10,11},
        {11,11,10,10,10,10,10,10},
        {11,11,11,10,10, 0,11, 0},
        {12,11,11, 0, 0, 0, 0, 0},
        {12,10,10, 0, 0,10,10, 0},
    },
    {//platform1_2
        {11,13,13,11,13,13,13,11},
        {12,13,12,12,12,13,13,12},
        {11,12,11,11,11,12,11,11},
        {11,10,12,11,11,10,11,10},
        {10,10,10,10,10,10,10,10},
        { 0,10,10, 0, 0, 0, 0, 0},
        { 0, 0, 0, 0,10,10, 0, 0},
        { 0,11, 0, 0, 0, 0, 0, 0},
    },
    {//platform1_3
        {13,13,11,13,13,11,13,13},
        {13,12,12,13,13,12,12,13},
        {12,11,11,12,11,11,12,12},
        {10,11,11,10,11,11,11,12},
        {10,10,10,10,10,10,11,12},
        { 0,10,10, 0, 0,11,11,11},
        { 0, 0, 0, 0, 0,11,11,11},
        {11, 0,10,10, 0,10,10,12},
    },
    {//platform1_4
        {11,11,11, 0, 0, 0, 0, 0},
        {11,11,10, 0,10,10, 0, 0},
        {12,11,11, 0,10, 0, 0, 0},
        {12,10,10, 0, 0, 0, 0, 0},
        {12,11,10, 0, 0, 0, 0,10},
        {11,11,10, 0,10,11, 0, 0},
        {12,11,11, 0,10,10, 0, 0},
        {12,10,10, 0, 0, 0, 0, 0},
    },
    {//platform1_5
        { 0, 0, 0, 0,11, 0, 0, 0},
        { 0,10,10, 0, 0, 0, 0, 0},
        { 0, 0, 0, 0, 0, 0, 0, 0},
        { 0, 0, 0, 0, 0,10,10, 0},
        { 0, 0, 0, 0, 0, 0, 0, 0},
        {11, 0, 0, 0, 0, 0,11, 0},
        { 0, 0,10,10, 0, 0, 0, 0},
        { 0, 0, 0, 0, 0, 0, 0, 0},
    },
    {//platform1_6
        { 0, 0, 0, 0, 0,11,11,12},
        { 0, 0, 0, 0, 0,10,11,11},
        { 0,10,10, 0, 0,11,11,11},
        { 0, 0, 0, 0,11,10,10,12},
        { 0, 0, 0, 0, 0,10,11,12},
        { 0, 0,10,10, 0,10,11,11},
        { 0, 0,10,10, 0,11,11,12},
        { 0, 0, 0, 0, 0,10,10,12},
    },
    {//stalactite                                                                                     24
        { 0,10,10,10,10,11,10, 0},
        { 0, 0,10,11, 0,11, 0, 0},
        { 0, 0, 0,11, 0,11, 0, 0},
        { 0, 0, 0,11, 0,10, 0, 0},
        { 0, 0, 0,10, 0, 0, 0, 0},
        { 0, 0, 0, 0, 0, 0, 0, 0},
        { 0, 0, 0, 0, 0, 0, 0, 0},
        { 0, 0, 0, 0, 0, 0, 0, 0},
    },
    {//ladder                                                                                         25
        { 0,12,12,12,12,12,12, 0},
        { 0, 0,11, 0, 0,11, 0, 0},
        { 0,12,12,12,12,12,12, 0},
        { 0, 0,11, 0, 0,11, 0, 0},
        { 0,12,12,12,12,12,12, 0},
        { 0, 0,11, 0, 0,11, 0, 0},
        { 0,12,12,12,12,12,12, 0},
        { 0, 0,11, 0, 0,11, 0, 0},
    },
    {//puddle                                                                                         26
        { 1, 1, 1, 1, 1, 1, 1, 1},
        { 9, 9, 9, 9, 9, 9, 9, 9},
        { 1, 9, 9, 9, 9, 9, 9, 9},
        { 9, 9, 9, 9, 9, 9, 9, 9},
        { 9, 9, 9, 9, 9, 9, 9, 9},
        { 9, 9, 9, 9, 9, 9, 9, 9},
        { 9, 9, 9, 9, 9, 1, 9, 9},
        { 9, 9, 9, 9, 9, 9, 9, 9},
    },
    {//pipe                                                                                           27
        { 0, 0, 0, 0, 0, 0, 0, 0},
        { 0, 0,10,10,10,10, 0, 0},
        { 0,10, 0, 0, 0, 0,10, 0},
        { 0,10, 0, 0, 0, 0,10, 0},
        { 0,10, 0, 0, 0, 0,10, 0},
        { 0,10, 1, 1, 1, 1,10, 0},
        {10,10, 1, 9, 1, 1,10,10},
        { 0, 0, 9, 9, 1, 9, 0, 0},
    },
    {//stream                                                                                         28
        { 0, 0, 9, 9, 9, 9, 0, 0},
        { 0, 0, 1, 9, 9, 9, 0, 0},
        { 0, 0, 9, 9, 9, 9, 0, 0},
        { 0, 0, 9, 9, 9, 9, 0, 0},
        { 0, 0, 9, 9, 9, 9, 0, 0},
        { 0, 0, 9, 9, 9, 9, 0, 0},
        { 0, 0, 9, 9, 1, 9, 0, 0},
        { 0, 0, 9, 9, 1, 9, 0, 0},        
    },
    {//empty
        { 0, 0, 0, 0, 0, 0, 0, 0},
        { 0, 0, 0, 0, 0, 0, 0, 0},
        { 0, 0, 0, 0, 0, 0, 0, 0},
        { 0, 0, 0, 0, 0, 0, 0, 0},
        { 0, 0, 0, 0, 0, 0, 0, 0},
        { 0, 0, 0, 0, 0, 0, 0, 0},
        { 0, 0, 0, 0, 0, 0, 0, 0},
        { 0, 0, 0, 0, 0, 0, 0, 0},
    }
};

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
const tile_index_t level[16][20] /*PROGMEM*/ =  {
    {{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{18, 0},{25, 0},{19, 0},{19, 0},{19, 0}},
    {{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{21, 0},{25, 0},{22, 0},{22, 0},{22, 0}},
    {{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{21, 0},{25, 0},{22, 0},{22, 0},{22, 0}},
    {{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{19, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{21, 0},{25, 0},{22, 0},{22, 0},{22, 0}},
    {{19, 0},{19, 0},{19, 0},{19, 0},{19, 0},{22, 0},{19, 0},{19, 0},{19, 0},{19, 0},{20, 0},{29, 0},{29, 0},{29, 0},{29, 0},{21, 0},{25, 0},{22, 0},{22, 0},{22, 0}},
    {{22, 0},{22, 0},{22, 0},{22, 0},{22, 0},{22, 0},{22, 0},{22, 0},{22, 0},{22, 0},{23, 0},{29, 0},{29, 0},{29, 0},{29, 0},{21, 0},{25, 0},{22, 0},{22, 0},{22, 0}},
    {{22, 0},{22, 0},{22, 0},{22, 0},{22, 0},{22, 0},{22, 0},{22, 0},{22, 0},{22, 0},{23, 0},{29, 0},{29, 0},{29, 0},{29, 0},{21, 0},{25, 0},{22, 0},{22, 0},{22, 0}},
    {{22, 0},{22, 0},{22, 0},{22, 0},{22, 0},{22, 0},{22, 0},{22, 0},{22, 0},{22, 0},{23, 0},{29, 0},{29, 0},{29, 0},{29, 0},{21, 0},{25, 0},{22, 0},{22, 0},{22, 0}},
    {{24, 0},{24, 0},{24, 0},{24, 0},{24, 0},{24, 0},{24, 0},{24, 0},{24, 0},{24, 0},{24, 0},{29, 0},{29, 0},{29, 0},{29, 0},{21, 0},{25, 0},{22, 0},{22, 0},{22, 0}},
    {{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{25, 0},{22, 0},{22, 0},{22, 0}},
    {{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{25, 0},{22, 0},{22, 0},{22, 0}},
    {{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{18, 0},{19, 0},{19, 0},{22, 0},{22, 0},{22, 0}},
    {{29, 0},{29, 0},{29, 0},{27, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{21, 0},{22, 0},{22, 0},{22, 0},{22, 0},{22, 0}},
    {{29, 0},{29, 0},{29, 0},{28, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{27, 0},{29, 0},{29, 0},{21, 0},{22, 0},{22, 0},{22, 0},{22, 0},{22, 0}},
    {{19, 0},{20, 0},{29, 0},{28, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{29, 0},{28, 0},{29, 0},{29, 0},{21, 0},{22, 0},{22, 0},{22, 0},{22, 0},{22, 0}},
    {{22, 0},{23, 0},{26, 0},{26, 0},{26, 0},{26, 0},{26, 0},{26, 0},{26, 0},{26, 0},{26, 0},{26, 0},{26, 0},{26, 0},{21, 0},{22, 0},{22, 0},{22, 0},{22, 0},{22, 0}},
};


#define TFT_CS     10
#define TFT_RST    8
#define TFT_DC     9

TFT_ST7735 tft = TFT_ST7735(TFT_CS,  TFT_DC, TFT_RST);
InputManager inputManager;

int16_t x = 0;
int16_t y = 100;
uint8_t frame = 0;

float posX = 0.0f;
float posY = 0.0f;
float oldPosX = -1.0f;
float oldPosY = -1.0f;
float velX = 0.0f;
float velY = 0.0f;
uint8_t animFrame = 0;
bool onGround = false;
bool flipSprite = false;

#define SIZE 8
#define ANIM_DELAY 60
#define DRAW_TILE drawTile1X

#define WALK_SPEED                  1.5f
#define GRAVITY                     1.2f
#define JUMP_FORCE                  8.0f


//a way to render at double size, maybe wont look too blocky
void drawTile2X(uint8_t index, uint8_t x, uint8_t y, uint8_t size) {
    tft.setArea(x, y, x+size-1, y+size-1);
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 2; j++) {
            tile_row_t r{ .packed = pgm_read_dword_near(&tiles[index][i]) };
            tft.pushColor(pgm_read_word_near(&colors[r.row.col1]));tft.pushColor(pgm_read_word_near(&colors[r.row.col1]));
            tft.pushColor(pgm_read_word_near(&colors[r.row.col2]));tft.pushColor(pgm_read_word_near(&colors[r.row.col2]));
            tft.pushColor(pgm_read_word_near(&colors[r.row.col3]));tft.pushColor(pgm_read_word_near(&colors[r.row.col3]));
            tft.pushColor(pgm_read_word_near(&colors[r.row.col4]));tft.pushColor(pgm_read_word_near(&colors[r.row.col4]));
            tft.pushColor(pgm_read_word_near(&colors[r.row.col5]));tft.pushColor(pgm_read_word_near(&colors[r.row.col5]));
            tft.pushColor(pgm_read_word_near(&colors[r.row.col6]));tft.pushColor(pgm_read_word_near(&colors[r.row.col6]));
            tft.pushColor(pgm_read_word_near(&colors[r.row.col7]));tft.pushColor(pgm_read_word_near(&colors[r.row.col7]));
            tft.pushColor(pgm_read_word_near(&colors[r.row.col8]));tft.pushColor(pgm_read_word_near(&colors[r.row.col8]));
        }
    }
}

void drawTile1X(uint8_t index, uint8_t x, uint8_t y, uint8_t size, bool flip = false) {
    tft.setArea(x, y, x+size-1, y+size-1);
    for (int i = 0; i < 8; i++) {
        tile_row_t r{ .packed = pgm_read_dword_near(&tiles[index][i]) };
        if (flip) {
            tft.pushColor(pgm_read_word_near(&colors[r.row.col8]));
            tft.pushColor(pgm_read_word_near(&colors[r.row.col7]));
            tft.pushColor(pgm_read_word_near(&colors[r.row.col6]));
            tft.pushColor(pgm_read_word_near(&colors[r.row.col5]));
            tft.pushColor(pgm_read_word_near(&colors[r.row.col4]));
            tft.pushColor(pgm_read_word_near(&colors[r.row.col3]));
            tft.pushColor(pgm_read_word_near(&colors[r.row.col2]));
            tft.pushColor(pgm_read_word_near(&colors[r.row.col1]));
        } else {
            tft.pushColor(pgm_read_word_near(&colors[r.row.col1]));
            tft.pushColor(pgm_read_word_near(&colors[r.row.col2]));
            tft.pushColor(pgm_read_word_near(&colors[r.row.col3]));
            tft.pushColor(pgm_read_word_near(&colors[r.row.col4]));
            tft.pushColor(pgm_read_word_near(&colors[r.row.col5]));
            tft.pushColor(pgm_read_word_near(&colors[r.row.col6]));
            tft.pushColor(pgm_read_word_near(&colors[r.row.col7]));
            tft.pushColor(pgm_read_word_near(&colors[r.row.col8]));
        }
    }
}

//shouldn't really keep these here but yeah..
/*
void playWalkAnim() {
    frame = 0;
    x = 0;
    y = 16;
    while(x < 80) {
        tft.fillRect(x, y, SIZE, SIZE, COLOR_BROWN_DARKER);
        x += 4;
        DRAW_TILE(frame, x, y, SIZE);
        frame = (frame + 1) % 4;
        delay(ANIM_DELAY);
    }
    tft.fillRect(x, y, SIZE, SIZE, COLOR_BROWN_DARKER);
}

void playIdleAnim() {
    frame = 4;
    x = 80;
    y = 16;
    for (int count = 0; count < 20; count++) {
        tft.fillRect(x, y, SIZE, SIZE, COLOR_BROWN_DARKER);
        DRAW_TILE(frame, x, y, SIZE);
        frame = ((frame + 1) % 4) + 4;
        delay(ANIM_DELAY+50);
    }
    tft.fillRect(x, y, SIZE, SIZE, COLOR_BROWN_DARKER);
}

void playJumpAnim() {
    frame = 10;
    x = 80;
    y = 16;
    for (int count = 0; count < 5; count++) {
        tft.fillRect(x, y, SIZE, SIZE, COLOR_BROWN_DARKER);
        y -= 3;
        x += 3;
        DRAW_TILE(frame, x, y, SIZE);
        frame = ((frame + 1) % 2) + 10;
        delay(ANIM_DELAY);
    }
    tft.fillRect(x, y, SIZE, SIZE, COLOR_BROWN_DARKER);
}

void playFallAnim() {
    frame = 8;
    x = 96;
    y = 0;
    while(y < 74) {
        tft.fillRect(x, y, SIZE, SIZE, COLOR_BROWN_DARKER);
        x = min(112, x+3);
        y += 6;
        DRAW_TILE(frame, x, y, SIZE);
        frame = ((frame + 1) % 2) + 8;
        delay(ANIM_DELAY);
    }
    tft.fillRect(x, y, SIZE, SIZE, COLOR_BROWN_DARKER);
}

void playClimbAnim() {
    frame = 12;
    x = 128;
    y = 80;
    while(y > 0) {
        DRAW_TILE(25, x, y, SIZE);
        y -= 3;
        DRAW_TILE(frame, x, y, SIZE);
        frame = ((frame + 1) % 2) + 12;
        delay(ANIM_DELAY+50);
    }
    tft.fillRect(x, y, SIZE, SIZE, COLOR_BROWN_DARKER);
}

void playCoinAnim() {
    frame = 14;
    x = 128;
    y = 0;
    while(1) {
        tft.fillRect(x, y, SIZE, SIZE, COLOR_BROWN_DARKER);
        DRAW_TILE(frame, x, y, SIZE);
        frame = ((frame + 1) % 4) + 14;
        delay(ANIM_DELAY+50);
    }
}
*/

uint8_t getTileAt(uint16_t x, uint16_t y) {
    return level[y>>3][x>>3].tile_index.index;
}

void draw() {
    tft.fillRect(oldPosX, oldPosY, SIZE, SIZE, COLOR_BROWN_DARKER);
    DRAW_TILE(animFrame>>2, posX, posY, SIZE, flipSprite);
}

void update(float dt) {
    oldPosX = posX;
    oldPosY = posY;

    if(inputManager.isButtonPressed(InputManager::Button::A) && onGround){
        velY = -JUMP_FORCE;
    }

    if (inputManager.isButtonPressed(InputManager::Button::LEFT)) {
        velX = -WALK_SPEED;
        flipSprite = true;
    } else if (inputManager.isButtonPressed(InputManager::Button::RIGHT)) {
        velX = WALK_SPEED;
        flipSprite = false;
    } else {
        velX = 0.0f;
    }

    velY = min(2*GRAVITY, velY + GRAVITY * dt);
    
    posX = min(max(0, posX + velX * dt), 152);
    posY += velY * dt;
    if(posY <= 0.0f) {
        velY = 0.0f;
        posY = 0.0f;
    }
    posY = min(posY, 120);

    if(velX <= 0) {
        if(getTileAt(posX + 0.0f, oldPosY + 0.0f) != 29 || getTileAt(posX + 0.0f, oldPosY + SIZE-1 /*0.9f*/) != 29) {
            posX = (((uint16_t)posX >> 3) + 1) << 3;
            velX = 0.0f;
        }
    } else {
        if(getTileAt(posX + SIZE /*1.0f*/, oldPosY + 0.0f) != 29 || getTileAt(posX + SIZE /*1.0f*/, oldPosY + SIZE-1 /*0.9f*/) != 29) {
            posX = ((uint16_t)posX >> 3) << 3;
            velX = 0.0f;
        }
    }

    onGround = false;
    if(velY <= 0) {
        if(getTileAt(posX + 0.0f, posY) != 29 || getTileAt(posX + SIZE-1 /*0.9f*/, posY) != 29) {
            posY = (((uint16_t)posY >> 3) + 1) << 3;
            velY = 0.0f;
        }
    } else {
        if(getTileAt(posX + 0.0f, posY + SIZE /*1.0f*/) != 29 || getTileAt(posX + SIZE-1 /*0.9f*/, posY + SIZE/*1.0f*/) != 29) {
            posY = ((uint16_t)posY >> 3) << 3;
            velY = 0.0f;
            onGround = true;
            if(posY >= 120) {
                velY = 0.0f;
                posY = 120;
                onGround = true;
            }
        }
    }

    //implement proper animation handling
    //  decoupled from framerate
    //  time based, anim frame rate configurable
    //  change animation based on eventual player FSM
    animFrame++;
    if(animFrame >> 2 >= 4)
        animFrame = 0;
}

//the game loop seems a bit meh, works for now
//maybe implement a more proper one, handle multiple "physics steps" before redrawing again
//not sure if a fixed timestep is really needed but could be a nice experiment
void game_loop() {
    int targetFrameTicks = 1000 / 30;
    uint32_t prevTicks = 0;
    uint32_t newTicks = 0;
    uint32_t frameTicks = 0;
    int32_t difference;
    float delta;

    prevTicks = millis();
    while(1) {
        newTicks = millis();
        frameTicks = newTicks - prevTicks;
        prevTicks = newTicks;
        delta = frameTicks / (float)targetFrameTicks;

        inputManager.processInput();
        update(delta);
        draw();

        difference = targetFrameTicks - frameTicks;
        if(difference > 0)
            delay(difference);
    }
}

void setup() {
    Serial.begin(9600);

    tft.begin();
    tft.setRotation(3);

    tft.fillScreen(COLOR_BROWN_DARKER);
    tft.fillRect(0, 0, 20, 20, COLOR_RED);
    for (int i = 0; i < 16; i++) {
        for(int j = 0; j < 20; j++) {
            if(i*SIZE >= 128 || j*SIZE >= 160)
                continue;
            //tile_index_t tile_index {.packed = pgm_read_byte_near(&level[i][j])};
            //DRAW_TILE(tile_index.tile_index.index,  j*SIZE, i*SIZE, SIZE);
            DRAW_TILE(level[i][j].tile_index.index,  j*SIZE, i*SIZE, SIZE);
        }
    }

    //tft.defineScrollArea(0, 160);
    //tft.fillRect(0, 0, 128, 10, COLOR_ORANGE);

    game_loop();

}

//int t = 0;

void loop() {
    // /!\ This is really IMPORTANT!!
    // find a way to implement continous side scrolling
    //  1. main issue being screen must be set to rotation 0 in order for scroll to work
    //  2. find a way to draw the next column of the level while the level is scrolling

    //tft.scroll(t);
    //t = (t+1) % 160;
    //delay(60);
}
