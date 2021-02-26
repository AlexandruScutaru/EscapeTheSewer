#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <Arduino.h>
#include <stdint.h>

#include <TFT_ST7735.h>

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

#define BG_COLOR                    COLOR_BROWN_DARKER


class Graphics {
public:
    Graphics();
    ~Graphics();

    void fillScreen(uint16_t color = BG_COLOR);
    void drawLevel();
    void drawTile(uint8_t index, uint16_t x, uint16_t y, uint8_t size, bool flip = false);
    void drawFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color = BG_COLOR);

    uint8_t getTileAt(uint16_t x, uint16_t y);

private:
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


    TFT_ST7735 mTFT;

    const static uint16_t colors[16] PROGMEM;
    const static tile_t tiles[] PROGMEM;
    const static tile_index_t level[16][20] /*PROGMEM*/;
};

#endif //GRAPHICS_H