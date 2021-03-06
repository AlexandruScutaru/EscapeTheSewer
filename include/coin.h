#ifndef COIN_H
#define COIN_H

#include "vec2.h"

#include <stdint.h>


class Graphics;

class Coin {
public:
    Coin();
    Coin(float x, float y);
    ~Coin();

    void update(float dt);
    void draw(Graphics& graphics);

    const vec2& getPos();

private:
    vec2 pos;
    uint8_t animFrameStart = 32;
    uint8_t animFrameCurrent = 0;
    uint8_t animFramesNumber = 4;
    uint32_t lastFrameUpdate = 0;

};

#endif // COIN_H