#ifndef STATUS_BAR_H
#define STATUS_BAR_H

#include <stdint.h>


class Graphics;

class StatusBar {
public:
    StatusBar();

    void update(float dt);
    void draw(Graphics& graphics, bool forceRedraw = false);

    void setPlayerHp(int8_t hp);
    void onRedraw();

private:
    bool readBatteryLevel();
    void drawBatteryIndicator(Graphics& graphics);
    void drawPlayerHpIndicator(Graphics& graphics);

    uint32_t lastBatteryReadTime;
    float batteryLevel = 0.1f;
    int8_t playerHp = 0;
    bool drawRequired = true;

};

#endif // STATUS_BAR_H