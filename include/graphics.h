#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "color_palette.h"

#include <stdint.h>

#include <TFT_ST7735.h>


class Graphics {
public:
    Graphics();
    ~Graphics();

    void fillScreen(uint16_t color = BG_COLOR);
    void drawTile(uint8_t index, uint16_t x, uint16_t y, uint8_t size, uint8_t flip = 0);
    void drawFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color = BG_COLOR);

    bool scroll(bool direction);

    static const int16_t max_game_area;

    struct Camera {
        uint8_t x1;
        uint8_t x2;
    };
    static Camera camera;

private:
    int16_t scrollAmount = 32; //on current state it means no scroll
    TFT_ST7735 mTFT;

};

#endif //GRAPHICS_H