#include "graphics_pc.h"
#include "input_manager_pc.h"
#include "logging.h"
#include "level.h"

#include <algorithm>

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

#define modulo(a, b) ((a)%(b)) < 0 ? (a)%(b) + (b) : (a)%(b)

const int Graphics::max_game_area = 128;
Graphics::Camera Graphics::camera = Graphics::Camera{0, Graphics::max_game_area >> 3};
sf::Clock Graphics::clock;

Graphics::Graphics() 
    : window(std::shared_ptr<sf::RenderWindow>(new sf::RenderWindow(sf::VideoMode(win_width, win_height), "Escape The Sewer")))
    , screen(std::vector<std::vector<uint16_t>>(screen_height, std::vector<uint16_t>(screen_width)))
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
    if(x < camera.x1 << 3)
        return;

    if(index == TILE_EMPTY) {
        drawFillRect(x, y, size, size, BG_COLOR);
        return;
    }
    x -= camera.x1 << 3;
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

void Graphics::printRow(std::vector<uint16_t>& row, int col) {
    sf::RectangleShape pixel(sf::Vector2f(1, 1));
    for (int i = 0; i < screen_width; i++) {
        pixel.setFillColor(RGB565toSfColor(screen[i][col]));
        pixel.setPosition(i, col);
        window->draw(pixel);
    }
}

void Graphics::display() {
    int tempScrollAmount = scrollAmount - scrollTop;
    if (tempScrollAmount < 0)
        tempScrollAmount = 0;

    for (int i = 0; i < scrollTop; i++)
        printRow(screen[i], i);

    for (int i = scrollTop + tempScrollAmount; i < scrollBottom; i++)
        printRow(screen[i], i);

    for (int i = 0; i < tempScrollAmount; i++)
        printRow(screen[i + scrollTop], i);

    for (int i = scrollBottom; i < screen_height; i++)
        printRow(screen[i], i);

    window->display();
}

void Graphics::drawFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    if(x < camera.x1 << 3)
        return;
    x -= camera.x1 << 3;
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

bool Graphics::scroll(bool direction) {
    if (direction) {
        if (camera.x2 + 1 > Level::levelW)
            return false;

        scrollAmount -= TILE_SIZE;
        if (scrollAmount < 0)
            scrollAmount = 19 << 3;
        backRow = scrollAmount >> 3;

        for (uint8_t i = 0; i < Level::levelH; i++) {
            const auto& tile_index = Level::getTileByIndex(i, camera.x2);
            drawTile(tile_index.tile_index.index, backRow * TILE_SIZE, i*TILE_SIZE, TILE_SIZE, tile_index.tile_index.flip);
        }
        camera.x1++;
        camera.x2++;

        return true;
    } else {
        if (camera.x1 == 0)
            return false;

        scrollAmount += TILE_SIZE;
        if (scrollAmount > 160)
            scrollAmount = TILE_SIZE;
        frontRow = (scrollAmount - 1) >> 3;

        camera.x1--;
        camera.x2--;

        for (uint8_t i = 0; i < Level::levelH; i++) {
            const auto& tile_index = Level::getTileByIndex(i, camera.x1);
            drawTile(tile_index.tile_index.index, frontRow << 3, i*TILE_SIZE, TILE_SIZE, tile_index.tile_index.flip);
        }
        return true;
    }
    return false;
}


/********* SHOULD BE REMOVED LATER, NOTES FOR ME TO REMEMBER THE HW SCROLLING *********/
/*

screenW = 16
screenH = 20

scrollTop = 4
scrollBottom = 20
scrollAmount = 0

frontRow = 0
backRow = 0

def initMemory(screenMemory):
    for row in range(screenH):
        screenMemory.append([row for i in range(screenW)])
    return screenMemory


def scroll(direction):
    global scrollAmount
    global frontRow
    global backRow

    if direction:
        scrollAmount -= 1
        if scrollAmount < 0:
            scrollAmount = 19
        backRow = scrollAmount
    else:
        scrollAmount += 1
        if scrollAmount > 20:
            scrollAmount = 1
        frontRow = scrollAmount - 1

    print("scroll: {} frontRow: {} backRow: {}".format(scrollAmount, frontRow, backRow))


def printScreen(screenMemory):
    # taking into account the scroll data values

    tempScrollAmount = scrollAmount - scrollTop
    if(tempScrollAmount < 0):
        tempScrollAmount = 0

    for i in range(scrollTop):
        printRow(screenMemory[i])

    for i in range(scrollTop + tempScrollAmount, scrollBottom):
        printRow(screenMemory[i])

    for i in range(tempScrollAmount):
        printRow(screenMemory[i + scrollTop])

    for i in range(scrollBottom, screenH):
        printRow(screenMemory[i])


def printRow(row):
    for col in row:
        print('{0:2}'.format(col), end=' ')
    print()


if __name__ == "__main__":
    screenMemory = initMemory([])

    for i in range(25):
        print("##################################################")
        scroll(True)
        printScreen(screenMemory)

*/