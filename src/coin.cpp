#include "coin.h"
#include "level.h"
#include "vec2.inl"

#if defined (ARDUINO) || defined (__AVR_ATmega328P__)
    #include "graphics.h"
    #include <Arduino.h>
#else
    #include "../pc_version/graphics_pc.h"
    #include "../pc_version/logging.h"

    #define millis() Graphics::getElapsedTime()
#endif

#define ANIM_FRAME_TIME  100

Coin::Coin() 
    : pos(vec2(40.0f, 16.0f))
{}

Coin::Coin(float x, float y)
    : pos(vec2(x, y))
{}

Coin::~Coin() {}


void Coin::update(float dt) {
}

void Coin::draw(Graphics& graphics) {
    graphics.drawTile(animFrameCurrent + animFrameStart, pos.x, pos.y, TILE_SIZE);
    
    if(lastFrameUpdate + ANIM_FRAME_TIME <= millis()) {
        animFrameCurrent = animFrameCurrent + 1 == animFramesNumber ? 0 : animFrameCurrent + 1;
        lastFrameUpdate = millis();
    }
}