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

#define IF_NON_COLLIDABLE(tile) if (tile.tile_index.index >= TILE_NON_COLLIDABLE_THRESHOLD)


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
    
    pos.x = min(max(0, pos.x + velocity.x * dt), (Level::levelW << 3) - TILE_SIZE);
    pos.y += velocity.y * dt;
    if(pos.y <= 0.0f) {
        velocity.y = 0.0f;
        pos.y = 0.0f;
    }
    pos.y = min(pos.y, 120);

    checkCollision();
}

void Slime::cleanPrevDraw(Graphics& graphics) {
    Level::cleanPrevDraw(oldPos);
}

void Slime::draw(Graphics& graphics) {
    if (pos.x < graphics.camera.x1 << 3 || pos.x + TILE_SIZE >= graphics.camera.x2 << 3)
        return;

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
    Level::collideWithLevel(pos, oldPos, velocity, vec2(WALK_SPEED, 0.0f), &flipSprite);
}