#include "slime.h"
#include "level.h"
#include "vec2.inl"

#if defined (ARDUINO) || defined (__AVR_ATmega328P__)
    #include "graphics.h"
    #include <Arduino.h>
#else
    #include "../pc_version/graphics_pc.h"
    #include "../pc_version/logging.h"

    #define min(a,b) ((a)<(b)?(a):(b))
    #define max(a,b) ((a)>(b)?(a):(b))

    #define millis() Graphics::getElapsedTime()
#endif

#define WALK_SPEED       0.5f
#define GRAVITY          1.2f
#define VELOCITY_RECOVERY 0.2f

#define ANIM_FRAME_TIME  100


Slime::Slime()
    : Slime(0.0f, 0.0f)
{}

Slime::Slime(float x, float y)
    : pos(vec2(x, y))
    , oldPos(vec2(x, y))
    , velocity(vec2(WALK_SPEED, 0.0f))
{}

Slime::~Slime() {}


void Slime::update(float dt) {
    oldPos = pos;

    //I need to update this a mathematically smarter way to deal with impulses
    if (flipSprite) {
        if (velocity.x + WALK_SPEED > 0.01f)
            velocity.x -= VELOCITY_RECOVERY * dt;
        else if (velocity.x + WALK_SPEED < -0.01f)
            velocity.x += VELOCITY_RECOVERY * dt;
    } else {
        if (velocity.x - WALK_SPEED > 0.01f)
            velocity.x -= VELOCITY_RECOVERY * dt;
        else if (velocity.x - WALK_SPEED < -0.01f)
            velocity.x += VELOCITY_RECOVERY * dt;
    }

    velocity.y = min(2*GRAVITY, velocity.y + GRAVITY * dt);
    
    pos.x = min(max(0, pos.x + velocity.x * dt), 152);
    pos.y += velocity.y * dt;
    if(pos.y <= 0.0f) {
        velocity.y = 0.0f;
        pos.y = 0.0f;
    }
    pos.y = min(pos.y, 120);

    checkCollision();
}

void Slime::draw(Graphics& graphics) {
    graphics.drawFillRect(oldPos.x, oldPos.y, TILE_SIZE, TILE_SIZE, COLOR_BROWN_DARKER);
    graphics.drawTile(animFrameCurrent + animFrameStart, pos.x, pos.y, TILE_SIZE, flipSprite);

    if(lastFrameUpdate + ANIM_FRAME_TIME <= millis()) {
        animFrameCurrent = animFrameCurrent + 1 == animFramesNumber ? 0 : animFrameCurrent + 1;
        lastFrameUpdate = millis();
    }
}

const vec2& Slime::getPos() {
    return pos;
}

bool Slime::hit(int8_t dmg, int8_t force) {
    velocity.x = force;
    velocity.y -= 2.0f;
    hp -= dmg;
    return hp > 0;
}

void Slime::checkCollision() {
    if(velocity.x <= 0.0f) {
        if(Level::getTileByPosition(pos.x + 0.0f, oldPos.y + 0.0f) != 63 || Level::getTileByPosition(pos.x + 0.0f, oldPos.y + TILE_SIZE-1) != 63) {
            pos.x = (((uint16_t)pos.x >> 3) + 1) << 3;
            velocity.x = WALK_SPEED;
            flipSprite = false;
        }
    } else {
        if(Level::getTileByPosition(pos.x + TILE_SIZE, oldPos.y + 0.0f) != 63 || Level::getTileByPosition(pos.x + TILE_SIZE, oldPos.y + TILE_SIZE-1) != 63) {
            pos.x = ((uint16_t)pos.x >> 3) << 3;
            velocity.x = -WALK_SPEED;
            flipSprite = true;
        }
    }

    if(velocity.y <= 0.0f) {
        if(Level::getTileByPosition(pos.x + 0.0f, pos.y) != 63 || Level::getTileByPosition(pos.x + TILE_SIZE-1, pos.y) != 63) {
            pos.y = (((uint16_t)pos.y >> 3) + 1) << 3;
            velocity.y = 0.0f;
        }
    } else {
        if(Level::getTileByPosition(pos.x + 0.0f, pos.y + TILE_SIZE) != 63 || Level::getTileByPosition(pos.x + TILE_SIZE-1, pos.y + TILE_SIZE) != 63) {
            pos.y = ((uint16_t)pos.y >> 3) << 3;
            velocity.y = 0.0f;
            if(pos.y >= 120) {
                velocity.y = 0.0f;
                pos.y = 120;
            }
        }
    }
}