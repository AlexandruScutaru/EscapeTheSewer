#ifndef COIN_H
#define COIN_H

#include "vector2.h"

#include <stdint.h>


class Graphics;

class Coin {
public:
    Coin();
    ~Coin();

    void update(float dt);
    void draw(Graphics& graphics);

private:
    Vector2 pos;
    uint8_t animFrameStart = 32;
    uint8_t animFrameCurrent = 0;
    uint8_t animFramesNumber = 4;
    uint32_t lastFrameUpdate = 0;

};

#endif // COIN_H