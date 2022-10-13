#include "ST7735.h"

#include <util/delay.h>
#include <stdlib.h>

#define bitSet(value, bit) ((value) |= (1UL << (bit)))


TFT_ST7735::TFT_ST7735() {
    mMactrlData = 0;
    bitSet(mMactrlData, 7); 
    bitSet(mMactrlData, 6);
}

void TFT_ST7735::begin() {
    // set DC and RST pins as output
    DDRB |= _BV(PB0) | _BV(PB1);

    enableData();

    setRST();
    _delay_ms(10);
    unsetRST();
    _delay_ms(10);
    setRST();
    _delay_ms(10);

    selectCS();

    writecommand_cont(CMD_SWRESET);
    _delay_ms(122);

    writecommand_cont(CMD_SLPOUT);
    _delay_ms(5);

    writecommand_cont(CMD_FRMCTR1);
    writedata8_cont(0x00);
    writedata8_cont(0x06);
    writedata8_cont(0x03);

    writecommand_cont(CMD_FRMCTR2);
    writedata8_cont(0x01);
    writedata8_cont(0x2C);
    writedata8_cont(0x2D);

    writecommand_cont(CMD_FRMCTR3);
    writedata8_cont(0x01);
    writedata8_cont(0x2C);
    writedata8_cont(0x2D);

    writecommand_cont(CMD_DINVCTR);
    writedata8_cont(0x00);

    writecommand_cont(CMD_PWCTR1);
    writedata8_cont(0xA2);
    writedata8_cont(0x02);
    writedata8_cont(0x84);

    writecommand_cont(CMD_PWCTR2);
    writedata8_cont(0xC5);

    writecommand_cont(CMD_PWCTR3);
    writedata8_cont(0x0A);
    writedata8_cont(0x00);

    writecommand_cont(CMD_PWCTR4);
    writedata8_cont(0x8A);
    writedata8_cont(0x2A);

    writecommand_cont(CMD_PWCTR5);
    writedata8_cont(0x8A);
    writedata8_cont(0x2A);

    writecommand_cont(CMD_VCOMCTR1);
    writedata8_cont(0x3C);
    writedata8_cont(0x38);

    writecommand_cont(CMD_DINVOF);

    writecommand_cont(CMD_PIXFMT);
    writedata8_cont(0x05);

    writecommand_cont(CMD_GAMMASET);
    writedata8_cont(0x01);

    uint8_t pGammaSet[15]= {0x0F,0x31,0x2B,0x0C,0x0E,0x08,0x4E,0xF1,0x37,0x07,0x10,0x03,0x0E,0x09,0x00};
    writecommand_cont(CMD_PGAMMAC);
    for (uint8_t i=0;i<15;i++) {
        writedata8_cont(pGammaSet[i]);
    }

    uint8_t nGammaSet[15]= {0x00,0x0E,0x14,0x07,0x11,0x07,0x31,0xC1,0x48,0x08,0x0F,0x0C,0x31,0x36,0x0F};
    writecommand_cont(CMD_NGAMMAC);
    for (uint8_t i=0;i<15;i++) {
        writedata8_cont(nGammaSet[i]);
    }

    writecommand_cont(CMD_IDLEOF);

    writecommand_cont(CMD_CLMADRS);
    writedata16_cont(0x0000);
    writedata16_cont(TFT_ST7735_TFTWIDTH);

    writecommand_cont(CMD_PGEADRS);
    writedata16_cont(0x0000);
    writedata16_cont(TFT_ST7735_TFTHEIGHT);

    writecommand_cont(CMD_NORML);
    writecommand_last(CMD_DISPON);
    _delay_ms(1);

    writecommand_cont(CMD_RAMWR);

    deselectCS();
    _delay_ms(1);

    setRotation(0x00);

    clearMemory();
    defineScrollArea(0,0);
    fillScreen(0x0000);

    _delay_ms(30);
}

void TFT_ST7735::setRotation(uint8_t m) {
    mRot = m % 4;
    mMactrlData &= ~(0xF8); //clear bit 3..7
    if (mRot == 0){
        mWidth  = TFT_ST7735_TFTWIDTH;
        mHeight = TFT_ST7735_TFTHEIGHT;
        defineScrollArea(mScrollTop, mScrollBottom);
    } else if (mRot == 1){
        bitSet(mMactrlData, 6); 
        bitSet(mMactrlData, 5);
        mWidth  = TFT_ST7735_TFTHEIGHT;
        mHeight = TFT_ST7735_TFTWIDTH;
    } else if (mRot == 2){
        bitSet(mMactrlData, 7); 
        bitSet(mMactrlData, 6);
        mWidth  = TFT_ST7735_TFTWIDTH;
        mHeight = TFT_ST7735_TFTHEIGHT;
        defineScrollArea(mScrollTop, mScrollBottom);
    } else {
        bitSet(mMactrlData, 7); 
        bitSet(mMactrlData, 5);
        mWidth  = TFT_ST7735_TFTHEIGHT;
        mHeight = TFT_ST7735_TFTWIDTH;
    }

    if (TFT_ST7735_CSPACE > 0)
        bitSet(mMactrlData, 3);

    selectCS();
    writecommand_cont(CMD_MADCTL);
    writedata8_last(mMactrlData);
    writecommand_cont(CMD_RAMWR);
    deselectCS();
}

void TFT_ST7735::defineScrollArea(int16_t tfa, int16_t bfa, int16_t scrollInternalBufferHeight) {
    if (mRot != 0)
        return;

    mScrollInternalBufferHeight = scrollInternalBufferHeight;
    int16_t offset  = mScrollInternalBufferHeight - mHeight;
    bfa += offset;
    mScrollTop = tfa;
    mScrollBottom = mScrollInternalBufferHeight - bfa;
    uint16_t area = mScrollInternalBufferHeight - tfa - bfa;

    selectCS();
    writecommand_cont(CMD_VSCLLDEF);
    writedata16_cont(tfa);
    writedata16_cont(area);
    writedata16_last(bfa);
    deselectCS();
}

void TFT_ST7735::scroll(int16_t pointer) {
    if (mRot != 0)
        return;

    if (pointer >= mScrollTop && pointer <= mScrollBottom) {
        selectCS();
        writecommand_cont(CMD_VSSTADRS);
        writedata16_last(pointer);
        deselectCS();
    }
}

int16_t TFT_ST7735::getScrollTop() {
    return mScrollTop;
}

int16_t TFT_ST7735::getScrollBottom() {
    return mScrollBottom;
}

void TFT_ST7735::fillScreen(uint16_t color) {
    selectCS();
    setAddrWindow_cont(0, 0, mWidth - 1, mHeight - 1);
    pushColors_cont(color, TFT_ST7735_CGRAM);
    deselectCS();
}

void TFT_ST7735::drawPixel(int16_t x, int16_t y, uint16_t color) {
    if (boundaryCheck(x,y))
        return;

    if ((x < 0) || (y < 0))
        return;

    selectCS();
    drawPixel_cont(x,y,color);
    deselectCS();
}

void TFT_ST7735::drawPixel_cont(int16_t x, int16_t y, uint16_t color) {
    setAddrWindow_cont(x, y, x + 1, y + 1);
    writedata16_cont(color);
}

void TFT_ST7735::drawFastHLine_cont(int16_t x, int16_t y, int16_t w, uint16_t color) {
    setAddrWindow_cont(x, y, x + w - 1, y);
    do {
        writedata16_cont(color);
    } while (--w > 0);
}

void TFT_ST7735::drawFastVLine_cont(int16_t x, int16_t y, int16_t h, uint16_t color) {
    setAddrWindow_cont(x, y, x, y + h - 1);
    do {
        writedata16_cont(color);
    } while (--h > 0);
}

void TFT_ST7735::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    if (boundaryCheck(x,y))
        return;

    w = sizeCheck(x, w, mWidth);
    h = sizeCheck(y, h, mHeight);
    selectCS();
    fillRect_cont(x,y,w,h,color);
    deselectCS();
}

void TFT_ST7735::fillRect_cont(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    if (w < 2 && h < 2) {
        drawPixel_cont(x, y, color); 
        return; 
    }

    if (h < 2) {
        drawFastHLine_cont(x, y, w, color);
        return;
    }

    if (w < 2) {
        drawFastVLine_cont(x, y, h, color);
        return; 
    }

    setAddrWindow_cont(x, y, (x+w)-1, (y+h)-1);
    pushColors_cont(color, w*h);
}

void TFT_ST7735::pushColor(uint16_t color) {
    selectCS();
    writedata16_last(color);
    deselectCS();
}

void TFT_ST7735::setArea(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    selectCS();
    setAddrWindow_cont(x0, y0, x1, y1);
    deselectCS();
}

bool TFT_ST7735::boundaryCheck(int16_t x, int16_t y) {
    return ((x >= mWidth) || (y >= mHeight));
}

int16_t TFT_ST7735::sizeCheck(int16_t origin, int16_t len, int16_t maxVal) {
    if (((origin + len) - 1) >= maxVal)
        len = maxVal - origin;
    return len;
}

void TFT_ST7735::pushColors_cont(uint16_t data, uint32_t times){
    enableData();
    while (times--) {
        for (uint8_t i = 0; i < 2; i++) {
            while (!(SPSR & (1 << SPIF)));
            SPDR = (data >> (8 - (i*8)));
        }
    }
    while(!(SPSR & (1 << SPIF)));
}

void TFT_ST7735::writecommand_cont(const uint8_t c) {
    enableCommand();
    spiWrite8(c);
}

void TFT_ST7735::writedata8_cont(uint8_t c) {
    enableData();
    spiWrite8(c);
}

void TFT_ST7735::writedata16_cont(uint16_t d) {
    enableData();
    spiWrite16(d);
}

void TFT_ST7735::writecommand_last(const uint8_t c) {
    enableCommand();
    spiWrite8(c);
    deselectCS();
}

void TFT_ST7735::writedata8_last(uint8_t c) {
    enableData();
    spiWrite8(c);
    deselectCS();
}

void TFT_ST7735::writedata16_last(uint16_t d) {
    enableData();
    spiWrite16(d);
    deselectCS();
}

void TFT_ST7735::setAddrWindow_cont(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    writecommand_cont(CMD_CLMADRS);
    writedata16_cont(x0);
    writedata16_cont(x1);
    writecommand_cont(CMD_PGEADRS);
    writedata16_cont(y0);
    writedata16_cont(y1);
    writecommand_cont(CMD_RAMWR);
}

void TFT_ST7735::clearMemory() {
    selectCS();
    setAddrWindow_cont(0, 0, mWidth, mHeight);
    pushColors_cont(0x0000, TFT_ST7735_CGRAM);
    deselectCS();
}
