#include "graphics.h"
#include "level.h"
#include "status_bar.h"

#include <SPI.h>

#define TFT_CS     10
#define TFT_RST    8
#define TFT_DC     9

#define DISPLAY_TFA                      0
#define DISPLAY_BFA                      0
#define DISPLAY_INTERNAL_BUFFER_HEIGHT 160

#define TOP_OFFSET TILE_SIZE

const int16_t Graphics::max_game_area = DISPLAY_WIDTH - DISPLAY_BFA;
Graphics::Camera Graphics::camera = Graphics::Camera{0, Graphics::max_game_area >> 3};

Graphics::Graphics() 
    : mTFT(TFT_ST7735(TFT_CS, TFT_DC, TFT_RST))
{
    mTFT.begin();
    mTFT.setRotation(0);
    mTFT.defineScrollArea(DISPLAY_TFA, DISPLAY_BFA, DISPLAY_INTERNAL_BUFFER_HEIGHT);
    scrollAmount = mTFT.getScrollTop();
}

Graphics::~Graphics() {}

void Graphics::fillScreen(uint16_t color) {
    mTFT.fillScreen(color);
}

void Graphics::drawTile(uint8_t index, uint16_t x, uint16_t y, uint8_t size, uint8_t flip) {
    if (x < (uint16_t)camera.x1 << 3u || x >= (uint16_t)camera.x2 << 3u)
        return;

    y += TOP_OFFSET;
    x %= DISPLAY_WIDTH;
    //guess in the end it is faster to do this check
    //maybe find a better place where it is checked a little less often
    if(index == TILE_EMPTY) {
        mTFT.fillRect(y, DISPLAY_WIDTH-x-size, size, size, BG_COLOR);
        return;
    }

    //check the case when we need to draw right over the seam of the hardware scrolling
    //draw first sprite reagardles of it being cropped (still losing some time on SPI pushing all colors, but ok for now)
    pushColors(index, y, DISPLAY_WIDTH-x-size, y+size-1, DISPLAY_WIDTH-1-x, size, flip, 0, size);

    //get the amount of cropped tile and push the rest over scroll seam
    int16_t offset = size - (DISPLAY_WIDTH - x);
    if (offset > 0){
        //mTFT.fillRect(y, 152+(size-offset), size, offset, COLOR_CYAN);
        pushColors(index, y, DISPLAY_WIDTH - offset, y+size-1, DISPLAY_WIDTH-1, size, flip, 0, offset);
    }
}

void Graphics::pushColors(uint8_t index, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t size, uint8_t flip, uint16_t from, uint16_t to) {
    mTFT.setArea(x0, y0, x1, y1);
    for (uint16_t i = from; i < to; i++) {
        Level::tile_row_t r;
        if (flip & 1) {
            r = Level::getTileRow(index, i);
        } else {
            r = Level::getTileRow(index, 7-i);
        }

        if (flip & (1 << 1))
            for (size_t col = 8; col > 0; col--)
                mTFT.pushColor(Level::colors[r.row[col]]);
        else
            for (size_t col = 1; col <= 8; col++)
                mTFT.pushColor(Level::colors[r.row[col]]);
    }
}

void Graphics::drawFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    y += TOP_OFFSET;
    x %= DISPLAY_WIDTH;
    mTFT.fillRect(y, DISPLAY_WIDTH-x-w, h, w, color);

    //get the amount of cropped rect and push the rest over scroll seam
    int16_t offset = w - (DISPLAY_WIDTH - x);
    if (offset > 0) {
        mTFT.fillRect(y, DISPLAY_WIDTH-offset, h, offset, color);
    }
}

bool Graphics::scroll(bool direction) {
    if (direction) {
        if (camera.x2 + 1 > Level::levelW) {
           return false;
        }

        scrollAmount -= TILE_SIZE;
        if (scrollAmount < mTFT.getScrollTop()) {
            scrollAmount = mTFT.getScrollBottom() - 8;
        }

        mTFT.scroll(scrollAmount);

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
        if (camera.x1 == 0) {
           return false;
        }

        scrollAmount += TILE_SIZE;
        if (scrollAmount >= mTFT.getScrollBottom()) {
            scrollAmount = mTFT.getScrollTop();
        }

        camera.x1--;
        camera.x2--;

        mTFT.scroll(scrollAmount);

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

int16_t Graphics::getScrollPivot() {
    return scrollPivotRow;
}

void Graphics::registerEvent(Event<StatusBar> e) {
    event = e;
}
