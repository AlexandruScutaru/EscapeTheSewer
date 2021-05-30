#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "color_palette.h"
#include "event.h"

#include <stdint.h>

#include <TFT_ST7735.h>

class StatusBar;

class Graphics {
public:
    Graphics();
    ~Graphics();

    void fillScreen(uint16_t color = BG_COLOR);
    void drawTile(uint8_t index, uint16_t x, uint16_t y, uint8_t size, uint8_t flip = 0);
    void drawFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color = BG_COLOR);

    bool scroll(bool direction);
    int16_t getScrollPivot();

    void registerEvent(Event<StatusBar> e);

    static const int16_t max_game_area;

    struct Camera {
        uint8_t x1;
        uint8_t x2;
    };
    static Camera camera;

private:
    void pushColors(uint8_t index, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t size, uint8_t flip, uint16_t from, uint16_t to);

    TFT_ST7735 mTFT;
    int16_t scrollAmount;
    int16_t scrollPivotRow = 0;

    Event<StatusBar> event;
};

#endif //GRAPHICS_H