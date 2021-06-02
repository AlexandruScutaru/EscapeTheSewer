#include "graphics_pc.h"
#include "input_manager_pc.h"
#include "logging.h"
#include "level.h"
#include "status_bar.h"

#include <algorithm>

#define ROW(x, y, i, j)  screen[x][y + (j-1)] = Level::colors[r.row[i]];

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

#define modulo(a, b) ((a)%(b)) < 0 ? (a)%(b) + (b) : (a)%(b)

#define UNSCROLLABLE_AMOUNT 0
#define TOP_OFFSET TILE_SIZE

const int Graphics::max_game_area = 160 - UNSCROLLABLE_AMOUNT;
Graphics::Camera Graphics::camera = Graphics::Camera{0, Graphics::max_game_area >> 3};
sf::Clock Graphics::clock;

Graphics::Graphics() 
    : window(std::shared_ptr<sf::RenderWindow>(new sf::RenderWindow(sf::VideoMode(win_width, win_height), "Escape The Sewer")))
    , screen(std::vector<std::vector<uint16_t>>(screen_height, std::vector<uint16_t>(screen_width)))
    , scrollAmount(UNSCROLLABLE_AMOUNT)
    , scrollTop(UNSCROLLABLE_AMOUNT)
{
    sf::View view(sf::FloatRect(0, 0, screen_height, screen_width));
    view.zoom(1.0);
    window->setView(view);

    fillScreen();
}

Graphics::~Graphics() {}


void Graphics::fillScreen(uint16_t color) {
    for (int i = 0; i < screen_height; i++) {
        for (int j = 0; j < max_game_area; j++) {
            screen[i][j] = BG_COLOR;
        }
    }
}

void Graphics::drawTile(uint8_t index, uint16_t x, uint16_t y, uint8_t size, uint8_t flip) {
    if (x < (uint16_t)camera.x1 << 3u || x >= (uint16_t)camera.x2 << 3u)
        return;

    if(index == TILE_EMPTY) {
        drawFillRect(x, y, size, size, BG_COLOR);
        return;
    }

    y += TOP_OFFSET;
    for (int i = 0; i < 8; i++) {
        Level::tile_row_t r;
        if (flip & 1) {
            r = Level::getTileRow(index, 7-i);
        } else {
            r = Level::getTileRow(index, i);
        }

        if (flip & (1 << 1)) {
            DRAW_ROW_FLIP((x + i)%160, y)
        } else {
            DRAW_ROW((x + i)%160, y)
        }
    }
}

void Graphics::drawFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    y += TOP_OFFSET;
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            screen[(x + j)%160][y + i] = color;
        }
    }
}

void Graphics::clear() {
    window->clear();
}

void Graphics::printRow(const std::vector<uint16_t>& screenRow) {
    sf::RectangleShape pixel(sf::Vector2f(1, 1));
    for (int i = 0; i < screen_width; i++) {
        pixel.setFillColor(RGB565toSfColor(screenRow[i]));
        pixel.setPosition(currentOutputRow, i);
        window->draw(pixel);
    }
    currentOutputRow++;
}

void Graphics::display() {
    currentOutputRow = 0;
    int top = scrollTop - UNSCROLLABLE_AMOUNT;
    int bottom = scrollBottom - UNSCROLLABLE_AMOUNT;
    int amount = 160 - scrollAmount;

    for (int i = 0; i < top; i++) {
        printRow(screen[i]);
    }

    for (int i = top + amount; i < bottom; i++) {
        printRow(screen[i]);
    }

    for (int i = 0; i < amount; i++) {
        printRow(screen[i + top]);
    }

    for (int i = bottom; i < screen_height; i++) {
        printRow(screen[i]);
    }

    window->display();
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

bool Graphics::scroll(bool direction) {
    if (direction) {
        if (camera.x2 + 1 > Level::levelW)
            return false;

        scrollAmount -= TILE_SIZE;
        if (scrollAmount < UNSCROLLABLE_AMOUNT)
            scrollAmount = 152;

        for (uint8_t i = 0; i < Level::levelH; i++) {
            const auto& tile_index = Level::getTileByIndex(i, camera.x2);
            drawTile(tile_index.tile_index.index, scrollPivotRow, i*TILE_SIZE, TILE_SIZE, tile_index.tile_index.flip);
        }

        camera.x1++;
        camera.x2++;
        scrollPivotRow += TILE_SIZE;

        if (event) event();
        return true;
    } else {
        if (camera.x1 == 0)
            return false;

        scrollAmount += TILE_SIZE;
        if (scrollAmount > 159)
            scrollAmount = UNSCROLLABLE_AMOUNT;

        camera.x1--;
        camera.x2--;

        for (uint8_t i = 0; i < Level::levelH; i++) {
            const auto& tile_index = Level::getTileByIndex(i, camera.x1);
            drawTile(tile_index.tile_index.index, scrollPivotRow-TILE_SIZE, i*TILE_SIZE, TILE_SIZE, tile_index.tile_index.flip);
        }

        scrollPivotRow -= TILE_SIZE;

        if (event) event();
        return true;
    }
    return false;
}

int Graphics::getScrollPivot() {
    return scrollPivotRow;
}

void Graphics::registerEvent(Event<StatusBar> e) {
    event = e;
}
