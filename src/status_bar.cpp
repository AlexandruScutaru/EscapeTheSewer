#include "status_bar.h"
#include "color_palette.h"

#if defined (ARDUINO) || defined (__AVR_ATmega328P__)
    #include "graphics.h"
    #include <Arduino.h>

    #define BATTERY_PIN       A7
    #define VOLTAGE_EPSILON   0.2      /*should be empirically determined*/
    #define MAX_ANALOG_VALUE  1024.0f

    #define LOG(x)
#else
    #include "../pc_version/graphics_pc.h"
    #include "../pc_version/logging.h"

    #define millis() Graphics::getElapsedTime()
#endif

#define BATTERY_IND_BG        0xBDF7
#define BATTERY_IND_GREEN     0x07E4
#define BATTERY_IND_YELLOW    0xFFE0
#define BATTERY_IND_ORANGE    0xFCA0
#define BATTERY_IND_RED       0xF800

#define STATUS_BAR_BG         0x39C7

#define BATTERY_IND_SIZE      30
#define BATTERY_IND_POS       (157 - BATTERY_IND_SIZE)

int val = 0;

StatusBar::StatusBar()
    : lastBatteryReadTime(millis())
{}


void StatusBar::addCoin() {
    coins++;
    drawRequired = true;
}

//not really needing the delta time, but eh
void StatusBar::update(float dt) {
    if(millis() - lastBatteryReadTime >= 1000) {
        readBatteryLevel();
        drawRequired = true;
        lastBatteryReadTime = millis();
    }
}

void StatusBar::draw(Graphics& graphics) {
    //need something better than a bool
    //like an also an event when there was a scroll
    if(drawRequired) {
        //clean status area
        graphics.drawFillRect(0, -8, 160, 8, STATUS_BAR_BG);

#if defined (ARDUINO) || defined (__AVR_ATmega328P__)
        //some weird debug printing on screen, having issues with reading battery level
        //and everything is already soldered...
        graphics.print(val%10, 0, 100);
        graphics.print(val/10%10, 0, 92);
        graphics.print(val/100%10, 0, 84);
        graphics.print(val/1000%10, 0, 76);
#endif
        drawBatteryIndicator(graphics);
    }
}

void StatusBar::readBatteryLevel() {
#if defined (ARDUINO) || defined (__AVR_ATmega328P__)
    //read the battery pin
    val =  analogRead(BATTERY_PIN);
    float voltage = (5 * val / MAX_ANALOG_VALUE) + VOLTAGE_EPSILON;

    //map current voltage from max-min voltage to 100-10 percent charge
    //not accurate, but at least there is an indication on when charging is required
    batteryLevel = min( ((voltage - 3.6f) / (4.2f - 3.6f) * (1.0f - 0.1f) + 0.1f), 1.0f );

    if (batteryLevel <= 0.0f)
        batteryLevel = 0.1f;
#else
    batteryLevel -= 0.1f;
    if (batteryLevel < 0.0f)
        batteryLevel = 1.0f;
#endif
}

void StatusBar::drawBatteryIndicator(Graphics& graphics) {
    uint8_t pos = graphics.getScrollPivot() + BATTERY_IND_POS;

    graphics.drawFillRect(pos, -8, BATTERY_IND_SIZE, 8, BATTERY_IND_BG);
    graphics.drawFillRect(pos + BATTERY_IND_SIZE, -6, 2, 4, BATTERY_IND_BG);

    uint16_t color = BATTERY_IND_GREEN;
    if(batteryLevel >= 0.5f && batteryLevel < 0.75f)
        color = BATTERY_IND_YELLOW;
    else if(batteryLevel >= 0.25f && batteryLevel < 0.5f)
        color = BATTERY_IND_ORANGE;
    else if (batteryLevel < 0.25f)
        color = BATTERY_IND_RED;

    graphics.drawFillRect(pos + 1, -7, batteryLevel * BATTERY_IND_SIZE-2, 6, color);
    drawRequired = false;
}
