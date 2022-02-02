#include "status_bar.h"
#include "color_palette.h"

#if defined (ARDUINO) || defined (__AVR_ATmega328P__)
    #include "graphics.h"
    #include <Arduino.h>

    #define BATTERY_PIN       A3
    #define VOLTAGE_EPSILON   0.2      /*should be empirically determined*/
    #define MAX_ANALOG_VALUE  1024.0f

    #define LOG(x)
#else
    #include "../pc_version/pc_version/graphics_pc.h"
    #include "../pc_version/pc_version/logging.h"

    #define millis() Graphics::getElapsedTime()
#endif

bool FloatEquality(float a, float b, float epsilon) {
   return fabs(a - b) < epsilon;
}

#define STATUS_BAR_BG         0x39C7
#define IND_BG                0xBDF7

#define BATTERY_IND_GREEN     0x07E4
#define BATTERY_IND_YELLOW    0xFFE0
#define BATTERY_IND_ORANGE    0xFCA0
#define BATTERY_IND_RED       0xF800
#define BATTERY_IND_SIZE      30
#define BATTERY_IND_POS       (DISPLAY_WIDTH - 3 - BATTERY_IND_SIZE)

#define HP_IND_RED            0xF800
#define HP_IND_SIZE           50
#define HP_IND_POS            (0)

//settings this value this way may leed to some mistakes if different from the value in player, but it's ok
#define MAX_HP                100


StatusBar::StatusBar()
    : lastBatteryReadTime(millis())
{}

//not really needing the delta time, but eh
void StatusBar::update(float dt) {
    if(millis() - lastBatteryReadTime >= 1000) {
        drawRequired = readBatteryLevel();
        lastBatteryReadTime = millis();
    }
}

void StatusBar::draw(Graphics& graphics) {
    //need something better than a bool
    //like an also an event when there was a scroll
    if(drawRequired) {
        graphics.drawFillRect(0, -8, DISPLAY_WIDTH, 8, STATUS_BAR_BG);

        drawPlayerHpIndicator(graphics);
        drawBatteryIndicator(graphics);

        drawRequired = false;
    }
}

void StatusBar::setPlayerHp(int8_t hp) {
    if (playerHp != hp) {
        playerHp = hp;
        drawRequired = true;
    }
}

void StatusBar::fire() {
    drawRequired = true;
}

bool StatusBar::readBatteryLevel() {
    float oldVal = batteryLevel;

#if defined (ARDUINO) || defined (__AVR_ATmega328P__)
    // //seems arduino reports ~950 on a full battery charge
    // float voltage = (5 * analogRead(BATTERY_PIN) / MAX_ANALOG_VALUE) + VOLTAGE_EPSILON;
    // //map current voltage from max-min voltage to 100-10 percent charge
    // //not accurate, but at least there is an indication on when charging is required
    // batteryLevel = min( ((voltage - 3.6f) / (4.2f - 3.6f) * (1.0f - 0.1f) + 0.1f), 1.0f );

    //connected the battery pin input to a potentiometer to test it with different values
    batteryLevel = analogRead(BATTERY_PIN) / 1024.0f;

    if (batteryLevel < 0.0f)
        batteryLevel = 0.0f;
#else
    batteryLevel -= 0.1f;
    if (batteryLevel < 0.0f)
        batteryLevel = 1.0f;
#endif

    return !(FloatEquality(batteryLevel, oldVal, 0.001f));
}

void StatusBar::drawBatteryIndicator(Graphics& graphics) {
    uint16_t pos = graphics.getScrollPivot() + BATTERY_IND_POS;

    graphics.drawFillRect(pos, -8, BATTERY_IND_SIZE, 8, IND_BG);
    graphics.drawFillRect(pos + BATTERY_IND_SIZE, -6, 2, 4, IND_BG);

    uint16_t color = BATTERY_IND_GREEN;
    if(batteryLevel >= 0.5f && batteryLevel < 0.75f)
        color = BATTERY_IND_YELLOW;
    else if(batteryLevel >= 0.25f && batteryLevel < 0.5f)
        color = BATTERY_IND_ORANGE;
    else if (batteryLevel < 0.25f)
        color = BATTERY_IND_RED;

    graphics.drawFillRect(pos + 1, -7, static_cast<int16_t>(batteryLevel * BATTERY_IND_SIZE-2), 6, color);
}

void StatusBar::drawPlayerHpIndicator(Graphics& graphics) {
    uint16_t pos = graphics.getScrollPivot() + HP_IND_POS;

    graphics.drawFillRect(pos, -8, HP_IND_SIZE, 8, IND_BG);
    float factor = playerHp / (float)MAX_HP;
    graphics.drawFillRect(pos + 1, -7, static_cast<int16_t>(factor * HP_IND_SIZE-2), 6, HP_IND_RED);
}
