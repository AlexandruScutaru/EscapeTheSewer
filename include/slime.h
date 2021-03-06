#ifndef SLIME_H
#define SLIME_H

#include "vec2.h"

#include <stdint.h>


class Graphics;

class Slime {
public:
    Slime();
    Slime(float x, float y);
    ~Slime();

    void update(float dt);
    void draw(Graphics& graphics);

    const vec2& getPos();

private:
    void checkCollision();

    vec2 pos;
    vec2 oldPos;
    vec2 velocity;
    bool flipSprite = false;
    uint8_t animFrameStart = 48;
    uint8_t animFrameCurrent = 0;
    uint8_t animFramesNumber = 4;
    uint32_t lastFrameUpdate = 0;

};

#endif // SLIME_H