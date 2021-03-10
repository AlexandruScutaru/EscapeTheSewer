#include "graphics.h"
#include "level.h"

#include <SPI.h>

#define TFT_CS     10
#define TFT_RST    8
#define TFT_DC     9


Graphics::Graphics() 
    : mTFT(TFT_ST7735(TFT_CS, TFT_DC, TFT_RST))
{
    mTFT.begin();
    mTFT.setRotation(0);

    mTFT.defineScrollArea(0, 160);
}

Graphics::~Graphics() {}

void Graphics::fillScreen(uint16_t color) {
    mTFT.fillScreen(color);
}

void Graphics::drawTile(uint8_t index, uint16_t x, uint16_t y, uint8_t size, uint8_t flip) {
    //guess in the end it is faster to do this check
    //maybe find a better place where it is checked a little less often
    if(index == TILE_EMPTY) {
        mTFT.fillRect(y, 159-x-size, size, size, BG_COLOR);
        return;
    }

    mTFT.setArea(y, 159-x-size, y+size-1, 159-x);
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
    mTFT.fillRect(y, 159-x-w, h, w, color);
}
