#include "player.h"
#include "data.h"
#include "vector2.inl"

#if defined (ARDUINO) || defined (__AVR_ATmega328P__)
    #include "input_manager.h"
    #include "graphics.h"
    #include <Arduino.h>
#else
    #include "../pc_version/input_manager_pc.h"
    #include "../pc_version/graphics_pc.h"
    #include "../pc_version/logging.h"

    #define min(a,b) ((a)<(b)?(a):(b))
    #define max(a,b) ((a)>(b)?(a):(b))
#endif

#define WALK_SPEED       1.2f
#define GRAVITY          1.2f
#define JUMP_FORCE       8.0f


Player::Player() 
    : pos(Vector2(0.0f))
    , oldPos(Vector2(0.0f))
    , velocity(Vector2(-1.0f))
{}

Player::~Player() {}


void Player::update(InputManager& input, float dt) {
    oldPos = pos;

    if(input.isButtonPressed(InputManager::Button::A) && onGround){
        velocity.y = -JUMP_FORCE;
    }

    if (input.isButtonPressed(InputManager::Button::LEFT)) {
        velocity.x = -WALK_SPEED;
        flipSprite = true;
    } else if (input.isButtonPressed(InputManager::Button::RIGHT)) {
        velocity.x = WALK_SPEED;
        flipSprite = false;
    } else {
        velocity.x = 0.0f;
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
    
    updateAnimation();
    
    //implement proper animation handling
    //  decoupled from framerate
    //  time based, anim frame rate configurable
    //  change animation based on eventual player FSM
    ticksPerAnimFrame++;
    if (ticksPerAnimFrame > 4) {
        ticksPerAnimFrame = 0;
        animFrameCurrent++;
        if(animFrameCurrent >= animFramesNumber)
            animFrameCurrent = 0;
    }

}

void Player::draw(Graphics& graphics) {
    graphics.drawFillRect(oldPos.x, oldPos.y, TILE_SIZE, TILE_SIZE, COLOR_BROWN_DARKER);
    graphics.drawTile(animFrameCurrent + animFrameStart, pos.x, pos.y, TILE_SIZE, flipSprite);
}

void Player::checkCollision() {
    if(velocity.x <= 0.0f) {
        if(Data::getTileByPosition(pos.x + 0.0f, oldPos.y + 0.0f) != 29 || Data::getTileByPosition(pos.x + 0.0f, oldPos.y + TILE_SIZE-1) != 29) {
            pos.x = (((uint16_t)pos.x >> 3) + 1) << 3;
            velocity.x = 0.0f;
        }
    } else {
        if(Data::getTileByPosition(pos.x + TILE_SIZE, oldPos.y + 0.0f) != 29 || Data::getTileByPosition(pos.x + TILE_SIZE, oldPos.y + TILE_SIZE-1) != 29) {
            pos.x = ((uint16_t)pos.x >> 3) << 3;
            velocity.x = 0.0f;
        }
    }

    onGround = false;
    if(velocity.y <= 0.0f) {
        if(Data::getTileByPosition(pos.x + 0.0f, pos.y) != 29 || Data::getTileByPosition(pos.x + TILE_SIZE-1, pos.y) != 29) {
            pos.y = (((uint16_t)pos.y >> 3) + 1) << 3;
            velocity.y = 0.0f;
        }
    } else {
        if(Data::getTileByPosition(pos.x + 0.0f, pos.y + TILE_SIZE) != 29 || Data::getTileByPosition(pos.x + TILE_SIZE-1, pos.y + TILE_SIZE) != 29) {
            pos.y = ((uint16_t)pos.y >> 3) << 3;
            velocity.y = 0.0f;
            onGround = true;
            if(pos.y >= 120) {
                velocity.y = 0.0f;
                pos.y = 120;
                onGround = true;
            }
        }
    }
}

Player::MovDir Player::getMovingDirection() {
    MovDir md;

    if (velocity.y > 0.0f) {
        md = MovDir::S;
    } else if (velocity.y < 0.0f) {
        md = MovDir::N;
    } else {
        if (velocity.x > 0.0f) {
            md = MovDir::E;
        } else if (velocity.x < 0.0f) {
            md = MovDir::W;
        } else {
            md = MovDir::NONE;
        }
    }

    return md;
}

void Player::updateAnimation() {
    //this really should be handled by a sort of proper FSM
    MovDir md = getMovingDirection();

    //save on some duplicate checks
    if (mAnimState != AnimState::IDLE && md == MovDir::NONE) {
        mAnimState = AnimState::IDLE;
        animFrameCurrent = 0;
        animFrameStart = ANIM_IDLE_START;
        animFramesNumber = ANIM_IDLE_FRAMES;
        return;
    }

    switch (mAnimState) {
    case AnimState::IDLE:
        if(md != MovDir::NONE) {
            if (md == MovDir::N) {
                mAnimState = AnimState::JUMP;
                animFrameCurrent = 0;
                animFrameStart = ANIM_JUMP_START;
                animFramesNumber = ANIM_JUMP_FRAMES;
            } else if (md == MovDir::S) {
                mAnimState = AnimState::FALL;
                animFrameCurrent = 0;
                animFrameStart = ANIM_FALL_START;
                animFramesNumber = ANIM_FALL_FRAMES;
            } else {
                mAnimState = AnimState::WALK;
                animFrameCurrent = 0;
                animFrameStart = ANIM_WALK_START;
                animFramesNumber = ANIM_WALK_FRAMES;
            }
        }
        break;
    case AnimState::WALK:
        if (md != MovDir::W && md != MovDir::E) {
            if (md == MovDir::N) {
                mAnimState = AnimState::JUMP;
                animFrameCurrent = 0;
                animFrameStart = ANIM_JUMP_START;
                animFramesNumber = ANIM_JUMP_FRAMES;
            } else {
                mAnimState = AnimState::FALL;
                animFrameCurrent = 0;
                animFrameStart = ANIM_FALL_START;
                animFramesNumber = ANIM_FALL_FRAMES;
            }
        }
        break;
    case AnimState::JUMP:
        if (md != MovDir::N) {
            if (md == MovDir::S) {
                mAnimState = AnimState::FALL;
                animFrameCurrent = 0;
                animFrameStart = ANIM_FALL_START;
                animFramesNumber = ANIM_FALL_FRAMES;
            } else {
                mAnimState = AnimState::WALK;
                animFrameCurrent = 0;
                animFrameStart = ANIM_WALK_START;
                animFramesNumber = ANIM_WALK_FRAMES;
            }
        }
        break;
    case AnimState::FALL:
        if (md != MovDir::N && md != MovDir::S) {
            mAnimState = AnimState::WALK;
            animFrameCurrent = 0;
            animFrameStart = ANIM_WALK_START;
            animFramesNumber = ANIM_WALK_FRAMES;
        }
        break;
    default:
        //execution shouldn't reach this
        break;
    }
}