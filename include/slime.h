#ifndef SLIME_H
#define SLIME_H

#include "vector2.h"

#include <stdint.h>


class Graphics;

class Slime {
public:
    Slime();
    ~Slime();

    void update(float dt);
    void draw(Graphics& graphics);

private:
    void checkCollision();

    Vector2 pos;
    Vector2 oldPos;
    Vector2 velocity;
    bool flipSprite = false;
    uint8_t animFrameStart = 48;
    uint8_t animFrameCurrent = 0;
    uint8_t animFramesNumber = 4;
    uint32_t lastFrameUpdate = 0;

};

#endif // SLIME_H