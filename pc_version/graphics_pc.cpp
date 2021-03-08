#include "graphics_pc.h"
#include "input_manager_pc.h"
#include "logging.h"

#include "level.h"


#define ROW(x, y, i, j)  screen[x][y + (j-1)] = Level::colors[r.row.col ## i ];

#define DRAW_ROW(x, y) ROW(x, y, 1, 1)\
                       ROW(x, y, 2, 2)\
                       ROW(x, y, 3, 3)\
                       ROW(x, y, 4, 4)\
                       ROW(x, y, 5, 5)\
                       ROW(x, y, 6, 6)\
                       ROW(x, y, 7, 7)\
                       ROW(x, y, 8, 8)

#define DRAW_ROW_FLIP(x, y) ROW(x, y, 8, 1)\
                            ROW(x, y, 7, 2)\
                            ROW(x, y, 6, 3)\
                            ROW(x, y, 5, 4)\
                            ROW(x, y, 4, 5)\
                            ROW(x, y, 3, 6)\
                            ROW(x, y, 2, 7)\
                            ROW(x, y, 1, 8)


sf::Clock Graphics::clock;

Graphics::Graphics() 
    : window(std::shared_ptr<sf::RenderWindow>(new sf::RenderWindow(sf::VideoMode(win_width, win_height), "Escape The Sewer")))
{
    sf::View view(sf::FloatRect(0, 0, screen_height, screen_width));
    view.zoom(1.0);
    window->setView(view);
    fillScreen();
}

Graphics::~Graphics() {}


void Graphics::fillScreen(uint16_t color) {
    for (int i = 0; i < screen_height; i++) {
        for (int j = 0; j < screen_width; j++) {
            screen[i][j] = BG_COLOR;
        }
    }
}

void Graphics::drawTile(uint8_t index, uint16_t x, uint16_t y, uint8_t size, uint8_t flip) {
    for (int i = 0; i < 8; i++) {
        Level::tile_row_t r;
        if (flip & 1) {
            r = Level::getTileRow(index, 7-i);
        } else {
            r = Level::getTileRow(index, i);
        }

        if (flip & (1 << 1)) {
            DRAW_ROW_FLIP(x + i, y)
        } else {
            DRAW_ROW(x + i, y)
        }
    }
}

void Graphics::clear() {
    window->clear();
}

void Graphics::display() {
    sf::RectangleShape pixel(sf::Vector2f(1, 1));
    for (int i = 0; i < screen_height; i++) {
        for (int j = 0; j < screen_width; j++) {
            pixel.setFillColor(RGB565toSfColor(screen[i][j]));
            pixel.setPosition(i, j);
            window->draw(pixel);
        }
    }
    window->display();
}

void Graphics::drawFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            screen[x + j][y + i] = color;
        }
    }
}

sf::Color Graphics::RGB565toSfColor(uint16_t color) {
    uint8_t r = (color & 0xF800) >> 11;
    uint8_t g = (color & 0x07E0) >> 5;
    uint8_t b =  color & 0x001F;

    r = (r * 255) / 31;
    g = (g * 255) / 63;
    b = (b * 255) / 31;

    return sf::Color(r, g, b);
}

uint32_t Graphics::getElapsedTime() {
    return clock.getElapsedTime().asMilliseconds();
}

void Graphics::sleep(uint32_t ms) {
    sf::sleep(sf::milliseconds(ms));
}

void Graphics::pollEvents() {
    sf::Event event;
    while (window->pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window->close();
        }
    }
}