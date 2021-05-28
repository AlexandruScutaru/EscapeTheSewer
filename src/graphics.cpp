#include "graphics.h"
#include "level.h"

#include <SPI.h>

#define TFT_CS     10
#define TFT_RST    8
#define TFT_DC     9

#define UNSCROLLABLE_AMOUNT 0
#define TOP_OFFSET TILE_SIZE

const int16_t Graphics::max_game_area = 160 - UNSCROLLABLE_AMOUNT;
Graphics::Camera Graphics::camera = Graphics::Camera{0, Graphics::max_game_area >> 3};

Graphics::Graphics() 
    : mTFT(TFT_ST7735(TFT_CS, TFT_DC, TFT_RST))
    , scrollAmount(UNSCROLLABLE_AMOUNT)
{
    mTFT.begin();
    mTFT.setRotation(0);

    mTFT.defineScrollArea(UNSCROLLABLE_AMOUNT, 160);
}

Graphics::~Graphics() {}

void Graphics::fillScreen(uint16_t color) {
    mTFT.fillScreen(color);
}

void Graphics::drawTile(uint8_t index, uint16_t x, uint16_t y, uint8_t size, uint8_t flip) {
    y += TOP_OFFSET;
    x %= 160;
    //guess in the end it is faster to do this check
    //maybe find a better place where it is checked a little less often
    if(index == TILE_EMPTY) {
        mTFT.fillRect(y, 160-x-size, size, size, BG_COLOR);
        return;
    }

    //check the case when we need to draw right over the seam of the hardware scrolling
    //need to implement wrap
    mTFT.setArea(y, 160-x-size, y+size-1, 159-x);
    for (int i = 0; i < 8; i++) {
        Level::tile_row_t r;
        if (flip & 1) {
            r = Level::getTileRow(index, i);
        } else {
            r = Level::getTileRow(index, 7-i);
        }

        if (flip & (1 << 1)) {
            mTFT.pushColor(Level::colors[r.row.col8]);
            mTFT.pushColor(Level::colors[r.row.col7]);
            mTFT.pushColor(Level::colors[r.row.col6]);
            mTFT.pushColor(Level::colors[r.row.col5]);
            mTFT.pushColor(Level::colors[r.row.col4]);
            mTFT.pushColor(Level::colors[r.row.col3]);
            mTFT.pushColor(Level::colors[r.row.col2]);
            mTFT.pushColor(Level::colors[r.row.col1]);
        } else {
            mTFT.pushColor(Level::colors[r.row.col1]);
            mTFT.pushColor(Level::colors[r.row.col2]);
            mTFT.pushColor(Level::colors[r.row.col3]);
            mTFT.pushColor(Level::colors[r.row.col4]);
            mTFT.pushColor(Level::colors[r.row.col5]);
            mTFT.pushColor(Level::colors[r.row.col6]);
            mTFT.pushColor(Level::colors[r.row.col7]);
            mTFT.pushColor(Level::colors[r.row.col8]);
        }
    }
}

void Graphics::drawFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    y += TOP_OFFSET;
    x %= 160;
    mTFT.fillRect(y, 160-x-w, h, w, color);
}

bool Graphics::scroll(bool direction) {
    if (direction) {
        if (camera.x2 + 1 > Level::levelW) {
           return false;
        }

        scrollAmount -= TILE_SIZE;
        if (scrollAmount < UNSCROLLABLE_AMOUNT) {
            scrollAmount = 152;
        }

        mTFT.scroll(scrollAmount);

        for (uint8_t i = 0; i < Level::levelH; i++) {
            const auto& tile_index = Level::getTileByIndex(i, camera.x2);
            drawTile(tile_index.tile_index.index, scrollPivotRow, i*TILE_SIZE, TILE_SIZE, tile_index.tile_index.flip);
        }

        camera.x1++;
        camera.x2++;
        scrollPivotRow += TILE_SIZE;

        return true;
    } else {
        if (camera.x1 == 0) {
           return false;
        }

        scrollAmount += TILE_SIZE;
        if (scrollAmount > 159) {
            scrollAmount = UNSCROLLABLE_AMOUNT;
        }

        camera.x1--;
        camera.x2--;

        mTFT.scroll(scrollAmount);

        for (uint8_t i = 0; i < Level::levelH; i++) {
            const auto& tile_index = Level::getTileByIndex(i, camera.x1);
            drawTile(tile_index.tile_index.index, scrollPivotRow-TILE_SIZE, i*TILE_SIZE, TILE_SIZE, tile_index.tile_index.flip);
        }

        scrollPivotRow -= TILE_SIZE;

        return true;
    }

    return false;
}

int16_t Graphics::getScrollPivot() {
    return scrollPivotRow;
}
