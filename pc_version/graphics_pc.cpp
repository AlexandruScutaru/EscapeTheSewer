#include "graphics_pc.h"
#include "input_manager_pc.h"
#include "logging.h"

#include "data.h"


#define ROW(x, y, i, j) pixel.setFillColor(RGB565toSfColor(Data::colors[r.row.col ## i ]));pixel.setPosition(x + (j-1)*1, y);window->draw(pixel);

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


Graphics::Graphics() 
    : window(std::shared_ptr<sf::RenderWindow>(new sf::RenderWindow(sf::VideoMode(w, h), "Escape The Sewer")))
{
    sf::View view(sf::FloatRect(0, 0, 160, 128));
    view.zoom(1.0);
    window->setView(view);
}

Graphics::~Graphics() {}


void Graphics::fillScreen(uint16_t color) {
    window->clear(RGB565toSfColor(color));
}

void Graphics::drawLevel() {
    for (int i = 0; i < Data::levelH; i++) {
        for(int j = 0; j < Data::levelW; j++) {
            drawTile(Data::getTileByIndex(i, j),  j*8*1, i*8*1, 8);
        }
    }
}

void Graphics::drawTile(uint8_t index, uint16_t x, uint16_t y, uint8_t size, bool flip) {
    sf::RectangleShape pixel(sf::Vector2f(1, 1));
    for (int i = 0; i < 8; i++) {
        Data::tile_row_t r = Data::getTileRow(index, i);
        if (flip) {
            DRAW_ROW_FLIP(x, y + i*1)
        } else {
            DRAW_ROW(x, y + i*1)
        }
    }
}

void Graphics::drawFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    sf::RectangleShape pixel(sf::Vector2f(1, 1));
    pixel.setFillColor(RGB565toSfColor(color));
    pixel.setPosition(x, y);
    window->draw(pixel);
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