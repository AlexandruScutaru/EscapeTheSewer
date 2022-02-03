#ifndef GRAPHICS_PC_H
#define GRAPHICS_PC_H

#include <color_palette.h>
#include <event.h>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include <memory>
#include <vector>

#define DISPLAY_HEIGHT  128
#define DISPLAY_WIDTH   128

using RenderWindowPtr = std::shared_ptr<sf::RenderWindow>;

struct Level;
class StatusBar;

class Graphics {
public:
    Graphics(Level& level);
    ~Graphics();

    void clear();
    void display();
    void reset();

    void fillScreen(uint16_t color = BG_COLOR);
    void drawTile(uint8_t index, uint16_t x, uint16_t y, uint8_t size, uint8_t flip = 0);
    void drawFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color = BG_COLOR);

    bool scroll(bool direction);
    int getScrollPivot();

    void registerEvent(Event<StatusBar> e);

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
    void printRow(const std::vector<uint16_t>& screenRow);

    int scrollAmount;
    int scrollTop;
    int scrollBottom = DISPLAY_WIDTH;
    int scrollPivotRow = 0;
    int currentOutputRow = 0;

    const int win_width = DISPLAY_WIDTH * 4;
    const int win_height = DISPLAY_HEIGHT * 4;
    // these are swapped because of the way I transpose the data plotted on the physical display
    // in order to take advantage of vertical hardware scrolling while in landscape orientation
    // I may need to revisit these, they are getting cloudy even for me lately
    const int screen_width = DISPLAY_HEIGHT;
    const int screen_height = DISPLAY_WIDTH;
    RenderWindowPtr window;
    std::vector<std::vector<uint16_t>> screen;
    static sf::Clock clock;

    Event<StatusBar> redrawEvent;
    Level& mLevel;
};

#endif // GRAPHICS_PC_H
