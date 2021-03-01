#include "graphics.h"
#include "data.h"

#include <SPI.h>

#define TFT_CS     10
#define TFT_RST    8
#define TFT_DC     9


Graphics::Graphics() 
    : mTFT(TFT_ST7735(TFT_CS, TFT_DC, TFT_RST))
{
    mTFT.begin();
    mTFT.setRotation(3);
}

Graphics::~Graphics() {}

void Graphics::fillScreen(uint16_t color) {
    mTFT.fillScreen(color);
}

void Graphics::drawLevel() {
    for (int i = 0; i < Data::levelH; i++) {
        for(int j = 0; j < Data::levelW; j++) {
            drawTile(Data::getTileByIndex(i, j),  j*TILE_SIZE, i*TILE_SIZE, TILE_SIZE);
        }
    }
}

void Graphics::drawTile(uint8_t index, uint16_t x, uint16_t y, uint8_t size, bool flip) {
    mTFT.setArea(x, y, x+size-1, y+size-1);
    for (int i = 0; i < 8; i++) {
        Data::tile_row_t r = Data::getTileRow(index, i);
        if (flip) {
            mTFT.pushColor(Data::colors[r.row.col8]);
            mTFT.pushColor(Data::colors[r.row.col7]);
            mTFT.pushColor(Data::colors[r.row.col6]);
            mTFT.pushColor(Data::colors[r.row.col5]);
            mTFT.pushColor(Data::colors[r.row.col4]);
            mTFT.pushColor(Data::colors[r.row.col3]);
            mTFT.pushColor(Data::colors[r.row.col2]);
            mTFT.pushColor(Data::colors[r.row.col1]);
        } else {
            mTFT.pushColor(Data::colors[r.row.col1]);
            mTFT.pushColor(Data::colors[r.row.col2]);
            mTFT.pushColor(Data::colors[r.row.col3]);
            mTFT.pushColor(Data::colors[r.row.col4]);
            mTFT.pushColor(Data::colors[r.row.col5]);
            mTFT.pushColor(Data::colors[r.row.col6]);
            mTFT.pushColor(Data::colors[r.row.col7]);
            mTFT.pushColor(Data::colors[r.row.col8]);
        }
    }
}

void Graphics::drawFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    mTFT.fillRect(x, y, w, h, color);
}
