#ifndef GRAPHICS_PC_H
#define GRAPHICS_PC_H

#include "color_palette.h"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include <memory>
#include <vector>


using RenderWindowPtr = std::shared_ptr<sf::RenderWindow>;

class Graphics {
public:
    Graphics();
    ~Graphics();

    void clear();
    void display();

    void fillScreen(uint16_t color = BG_COLOR);
    void drawTile(uint8_t index, uint16_t x, uint16_t y, uint8_t size, uint8_t flip = 0);
    void drawFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color = BG_COLOR);

    void scroll(bool direction);

    RenderWindowPtr getWindow() {return window;}
    void pollEvents();
    static uint32_t getElapsedTime();
    void sleep(uint32_t ms);

    static const int max_game_area;

    struct Camera {
        uint8_t x1;
        uint8_t x2;
    };
    static Camera camera;

private:
    sf::Color RGB565toSfColor(uint16_t color);

    const int win_width = 640;
    const int win_height = 512;
    const int screen_width = 128;
    const int screen_height = 160;
    RenderWindowPtr window;
    std::vector<std::vector<uint16_t>> screen;
    int scrollAmount = 0;
    static sf::Clock clock;
};


#endif // GRAPHICS_PC_H