#ifndef GRAPHICS_PC_H
#define GRAPHICS_PC_H

#include "color_palette.h"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include <memory>


using RenderWindowPtr = std::shared_ptr<sf::RenderWindow>;

class Graphics {
public:
    Graphics();
    ~Graphics();

    void fillScreen(uint16_t color = BG_COLOR);
    void drawLevel();
    void drawTile(uint8_t index, uint16_t x, uint16_t y, uint8_t size, bool flip = false);
    void drawFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color = BG_COLOR);

    RenderWindowPtr getWindow() {return window;}
    void pollEvents();
    static uint32_t getElapsedTime();
    void sleep(uint32_t ms);

private:
    sf::Color RGB565toSfColor(uint16_t color);

    int w = 640;
    int h = 512;
    RenderWindowPtr window;

    static sf::Clock clock;
};


#endif // GRAPHICS_PC_H