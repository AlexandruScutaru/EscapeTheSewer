#pragma once

#include "ST7735_Config.h"

#include<avr/io.h>


class TFT_ST7735 {
public:
    TFT_ST7735();

    void begin();
    void setRotation(uint8_t rot);
    void defineScrollArea(int16_t tfa, int16_t bfa, int16_t scrollInternalBufferHeight = TFT_ST7735_TFTHEIGHT);
    void scroll(int16_t pointer);
    int16_t getScrollTop();
    int16_t getScrollBottom();

    void fillScreen(uint16_t color);
    void drawPixel(int16_t x, int16_t y, uint16_t color);
    void fillRect(int16_t x, int16_t y, int16_t w, int16_t h,uint16_t color);
    void pushColor(uint16_t color);
    void setArea(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);

 private:
    void __attribute__((always_inline)) enableData() {
        PORTB |= _BV(PB1);
    }

    void __attribute__((always_inline)) enableCommand() {
        PORTB &= ~_BV(PB1);
    }

    void __attribute__((always_inline)) setRST() {
        PORTB |= _BV(PB0);
    }

    void __attribute__((always_inline)) unsetRST() {
        PORTB &= ~_BV(PB0);
    }

    static void __attribute__((always_inline)) deselectCS() {
        PORTB |= _BV(PB2);
    }

    static void __attribute__((always_inline)) selectCS() {
        PORTB &= ~_BV(PB2);
    }

    static void __attribute__((always_inline)) spiWrite8(uint8_t data) {
        SPDR = data;
        while(!(SPSR & _BV(SPIF)));
    }

    static void __attribute__((always_inline)) spiWrite16(uint16_t data) {
        spiWrite8(data >> 8);
        spiWrite8(data);
    }

    void writecommand_cont(const uint8_t c);
    void writedata8_cont(uint8_t c);
    void writedata16_cont(uint16_t d);
    void writecommand_last(const uint8_t c);
    void writedata8_last(uint8_t c);
    void writedata16_last(uint16_t d);
    void setAddrWindow_cont(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
    void pushColors_cont(uint16_t data, uint32_t times);
    void clearMemory();

    void drawPixel_cont(int16_t x, int16_t y, uint16_t color);
    void drawFastHLine_cont(int16_t x, int16_t y, int16_t w, uint16_t color);
    void drawFastVLine_cont(int16_t x, int16_t y, int16_t h, uint16_t color);
    void fillRect_cont(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);

    bool boundaryCheck(int16_t x,int16_t y);
    int16_t sizeCheck(int16_t origin,int16_t len,int16_t maxVal);

    volatile uint8_t mMactrlData = 0;
    uint8_t mRot = 0;
    int16_t mWidth = TFT_ST7735_TFTWIDTH;
    int16_t mHeight = TFT_ST7735_TFTHEIGHT;
    int16_t mScrollTop = 0;
    int16_t mScrollBottom = 0;
    int16_t mScrollInternalBufferHeight = 0;

};
