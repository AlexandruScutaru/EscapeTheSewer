#pragma once

#include <stdint.h>

#define TFT_ST7735_TFTWIDTH     128
#define TFT_ST7735_TFTHEIGHT    128
#define TFT_ST7735_CGRAM        TFT_ST7735_TFTWIDTH * TFT_ST7735_TFTHEIGHT
#define TFT_ST7735_CSPACE       1

// Registers
#define CMD_NOP          0x00 // Non operation
#define CMD_SWRESET      0x01 // Soft Reset
#define CMD_SLPIN        0x10 // Sleep ON
#define CMD_SLPOUT       0x11 // Sleep OFF
#define CMD_PTLON        0x12 // Partial Mode ON
#define CMD_NORML        0x13 // Normal Display ON
#define CMD_DINVOF       0x20 // Display Inversion OFF
#define CMD_DINVON       0x21 // Display Inversion ON
#define CMD_GAMMASET     0x26 // Gamma Set
#define CMD_DISPOFF      0x28 // Display OFF
#define CMD_DISPON       0x29 // Display ON
#define CMD_IDLEON       0x39 // Idle Mode ON
#define CMD_IDLEOF       0x38 // Idle Mode OFF
#define CMD_CLMADRS      0x2A // Column Address Set
#define CMD_PGEADRS      0x2B // Page Address Set
#define CMD_RAMWR        0x2C // Memory Write
#define CMD_RAMRD        0x2E // Memory Read
#define CMD_PARTAREA     0x30 // Partial Area
#define CMD_VSCLLDEF     0x33 // Vertical Scroll Definition
#define CMD_TEFXLON      0x35 // Tearing Effect Line ON
#define CMD_TEFXLOF      0x34 // Tearing Effect Line OFF
#define CMD_MADCTL       0x36 // Memory Access Control
#define CMD_VSSTADRS     0x37 // Vertical Scrolling Start address
#define CMD_PIXFMT       0x3A // Interface Pixel Format
#define CMD_FRMCTR1      0xB1 // Frame Rate Control (In normal mode/Full colors)
#define CMD_FRMCTR2      0xB2 // Frame Rate Control(In Idle mode/8-colors)
#define CMD_FRMCTR3      0xB3 // Frame Rate Control(In Partial mode/full colors)
#define CMD_DINVCTR	     0xB4 // Display Inversion Control
#define CMD_DFUNCTR      0xB6 // Display Fuction set 5
#define CMD_PWCTR1       0xC0 // Power_Control1
#define CMD_PWCTR2       0xC1 // Power_Control2
#define CMD_PWCTR3       0xC2 // Power_Control3
#define CMD_PWCTR4       0xC3 // Power_Control4
#define CMD_PWCTR5       0xC4 // Power_Control5
#define CMD_PWCTR6       0xFC // Power_Control6
#define CMD_VCOMCTR1     0xC5 // VCOM_Control 1
#define CMD_VCOMCTR2     0xC7 // VCOM_Control 2
#define CMD_PGAMMAC	     0xE0 // Positive Gamma Correction Setting
#define CMD_NGAMMAC	     0xE1 // Negative Gamma Correction Setting

#define MADCTL_MY        0x80
#define MADCTL_MX        0x40
#define MADCTL_MV        0x20
#define MADCTL_ML        0x10
#define MADCTL_RGB       0x00
#define MADCTL_BGR       0x08
#define MADCTL_MH        0x04
