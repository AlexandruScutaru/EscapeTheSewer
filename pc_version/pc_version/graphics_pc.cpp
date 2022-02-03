#include "graphics_pc.h"
#include "input_manager_pc.h"

#include <level_utils.h>
#include <status_bar.h>

#include <algorithm>

#define ROW(level, x, y, i, j)  screen[x][y + (j-1)] = LevelUtils::getColor(level, r.row[i]);

#define DRAW_ROW(level, x, y) ROW(level, x, y, 1, 1)\
                              ROW(level, x, y, 2, 2)\
                              ROW(level, x, y, 3, 3)\
                              ROW(level, x, y, 4, 4)\
                              ROW(level, x, y, 5, 5)\
                              ROW(level, x, y, 6, 6)\
                              ROW(level, x, y, 7, 7)\
                              ROW(level, x, y, 8, 8)

#define DRAW_ROW_FLIP(level, x, y) ROW(level, x, y, 8, 1)\
                                   ROW(level, x, y, 7, 2)\
                                   ROW(level, x, y, 6, 3)\
                                   ROW(level, x, y, 5, 4)\
                                   ROW(level, x, y, 4, 5)\
                                   ROW(level, x, y, 3, 6)\
                                   ROW(level, x, y, 2, 7)\
                                   ROW(level, x, y, 1, 8)

#define modulo(a, b) ((a)%(b)) < 0 ? (a)%(b) + (b) : (a)%(b)

#define UNSCROLLABLE_AMOUNT 0
#define TOP_OFFSET TILE_SIZE

const int Graphics::max_game_area = DISPLAY_WIDTH - UNSCROLLABLE_AMOUNT;
Graphics::Camera Graphics::camera = Graphics::Camera{ 0, Graphics::max_game_area >> 3 };
sf::Clock Graphics::clock;

Graphics::Graphics(Level& level) 
    : window(std::shared_ptr<sf::RenderWindow>(new sf::RenderWindow(sf::VideoMode(win_width, win_height), "Escape The Sewer")))
    , screen(std::vector<std::vector<uint16_t>>(screen_height, std::vector<uint16_t>(screen_width)))
    , scrollAmount(UNSCROLLABLE_AMOUNT)
    , scrollTop(UNSCROLLABLE_AMOUNT)
    , mLevel(level)
{
    camera = Graphics::Camera {0, Graphics::max_game_area >> 3};
    sf::View view(sf::FloatRect(0.0f, 0.0f, static_cast<float>(screen_height), static_cast<float>(screen_width)));
    view.zoom(1.0);
    window->setView(view);

    fillScreen();
}

Graphics::~Graphics() {}

void Graphics::reset() {
    scrollPivotRow = 0;
    currentOutputRow = 0;
    scrollAmount = UNSCROLLABLE_AMOUNT;
    scrollTop = UNSCROLLABLE_AMOUNT;
    camera = Graphics::Camera { 0, Graphics::max_game_area >> 3 };
    fillScreen();
}

void Graphics::fillScreen(uint16_t color) {
    for (int i = 0; i < screen_height; i++) {
        for (int j = 0; j < screen_width; j++) {
            screen[i][j] = color;
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
            r = LevelUtils::getTileRow(mLevel, index, 7-i);
        } else {
            r = LevelUtils::getTileRow(mLevel, index, i);
        }

        if (flip & (1 << 1)) {
            DRAW_ROW_FLIP(mLevel, (x + i) % DISPLAY_WIDTH, y)
        } else {
            DRAW_ROW(mLevel, (x + i) % DISPLAY_WIDTH, y)
        }
    }
}

void Graphics::drawFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    y += TOP_OFFSET;
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            screen[(x + j) % DISPLAY_WIDTH][y + i] = color;
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
        pixel.setPosition(static_cast<float>(currentOutputRow), static_cast<float>(i));
        window->draw(pixel);
    }
    currentOutputRow++;
}

void Graphics::display() {
    currentOutputRow = 0;
    int top = scrollTop - UNSCROLLABLE_AMOUNT;
    int bottom = scrollBottom - UNSCROLLABLE_AMOUNT;
    int amount = DISPLAY_WIDTH - scrollAmount;

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
    if (!window->hasFocus()) {
        return;
    }

    while (window->pollEvent(event)) { 
        if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
            window->close();
        }
    }
}

bool Graphics::scroll(bool direction) {
    if (direction) {
        if (camera.x2 + 1 > mLevel.levelW)
            return false;

        scrollAmount -= TILE_SIZE;
        if (scrollAmount < UNSCROLLABLE_AMOUNT)
            scrollAmount = DISPLAY_WIDTH - 8;

        for (uint8_t i = 0; i < mLevel.levelH; i++) {
            const auto& tile_index = LevelUtils::getTileByIndex(mLevel, i, camera.x2);
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
        if (scrollAmount > DISPLAY_WIDTH - 1)
            scrollAmount = UNSCROLLABLE_AMOUNT;

        camera.x1--;
        camera.x2--;

        for (uint8_t i = 0; i < mLevel.levelH; i++) {
            const auto& tile_index = LevelUtils::getTileByIndex(mLevel, i, camera.x1);
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
