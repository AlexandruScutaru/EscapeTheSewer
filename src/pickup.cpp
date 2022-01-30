#include "pickup.h"
#include "level.h"
#include "vec2.inl"

#if defined (ARDUINO) || defined (__AVR_ATmega328P__)
    #include "graphics.h"
    #include <Arduino.h>
#else
    #include "../pc_version/pc_version/graphics_pc.h"
    #include "../pc_version/pc_version/logging.h"
#endif


Pickup::Pickup() {}

Pickup::Pickup(const vec2& mPos, uint8_t tile, uint8_t flip) 
    : mPos(mPos)
    , mTile(tile)
    , mFlip(flip)
{}

Pickup::~Pickup() {}

void Pickup::cleanPrevDraw(Level& level, Graphics& graphics) {
    graphics.drawTile(TILE_EMPTY, static_cast<uint16_t>(mPos.x), static_cast<uint16_t>(mPos.y), TILE_SIZE);
}

void Pickup::draw(Graphics& graphics) {
    if (mPos.x < graphics.camera.x1 << 3 || mPos.x + TILE_SIZE >= graphics.camera.x2 << 3)
        return;

    graphics.drawTile(mTile, static_cast<uint16_t>(mPos.x), static_cast<uint16_t>(mPos.y), TILE_SIZE, mFlip);
}

const vec2& Pickup::getPos() {
    return mPos;
}
