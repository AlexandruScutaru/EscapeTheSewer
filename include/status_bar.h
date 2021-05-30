#ifndef STATUS_BAR_H
#define STATUS_BAR_H

#include <stdint.h>


class Graphics;

class StatusBar {
public:
    StatusBar();

    void update(float dt);
    void draw(Graphics& graphics);

private:
    void readBatteryLevel();
    void drawBatteryIndicator(Graphics& graphics);

    uint32_t lastBatteryReadTime;
    float batteryLevel = 0.1f;
    bool drawRequired = true;

};

#endif // STATUS_BAR_H