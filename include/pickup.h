#ifndef PICKUP_H
#define PICKUP_H

#include "vec2.h"

#include <stdint.h>


class Graphics;

class Pickup {
public:
    Pickup();
    Pickup(const vec2& pos, uint8_t tile, uint8_t flip);
    ~Pickup();

    void draw(Graphics& graphics);

    const vec2& getPos();

private:
    vec2 mPos;
    uint8_t mTile;
    uint8_t mFlip;

};

#endif // PICKUP_H