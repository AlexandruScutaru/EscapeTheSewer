#include "player.h"
#include "level.h"
#include "vec2.inl"

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

    #define millis() Graphics::getElapsedTime()
#endif

#define WALK_SPEED       1.2f
#define GRAVITY          1.2f
#define JUMP_FORCE       8.0f

#define ANIM_FRAME_TIME  100


Player::Player() 
    : pos(vec2(0.0f, 16.0f))
    , oldPos(vec2(0.0f, 16.0f))
    , velocity(vec2(0.0f))
{
    changeAnimation(AnimState::IDLE);
}

Player::Player(float x, float y)
    : pos(vec2(x, y))
    , oldPos(vec2(x, y))
    , velocity(vec2(0.0f))
{
    changeAnimation(AnimState::IDLE);
}

Player::~Player() {}


void Player::setPos(const vec2& pos) {
    this->pos = pos;
}

void Player::update(InputManager& input, float dt) {
    oldPos = pos;

    if(input.wasButtonPressedNow(InputManager::Button::A) && onGround) {
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

    if (input.wasButtonPressedNow(InputManager::Button::B) && mAnimState != AnimState::ATTACK) {
        changeAnimation(AnimState::ATTACK);
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
}

void Player::draw(Graphics& graphics) {
    graphics.drawFillRect(oldPos.x, oldPos.y, TILE_SIZE, TILE_SIZE, COLOR_BROWN_DARKER);
    graphics.drawTile(animFrameCurrent + animFrameStart, pos.x, pos.y, TILE_SIZE, flipSprite);

    //add proper Timer implementation to be reused across app, with a callback as to avoid constant polling
    if(lastFrameUpdate + ANIM_FRAME_TIME <= millis()) {
        //jeezuhs.. damn this animation handling is bad
        //can't call it code smell.. it is an overwhelming stench
        if (mAnimState == AnimState::ATTACK && animFrameCurrent == 1) {
            changeAnimation(AnimState::IDLE);
        } else {
            animFrameCurrent = animFrameCurrent + 1 == animFramesNumber ? 0 : animFrameCurrent + 1;
        }
        lastFrameUpdate = millis();
    }

}

void Player::checkCollision() {
    if(velocity.x <= 0.0f) {
        if(Level::getTileByPosition(pos.x + 0.0f, oldPos.y + 0.0f) != 63 || Level::getTileByPosition(pos.x + 0.0f, oldPos.y + TILE_SIZE-1) != 63) {
            pos.x = (((uint16_t)pos.x >> 3) + 1) << 3;
            velocity.x = 0.0f;
        }
    } else {
        if(Level::getTileByPosition(pos.x + TILE_SIZE, oldPos.y + 0.0f) != 63 || Level::getTileByPosition(pos.x + TILE_SIZE, oldPos.y + TILE_SIZE-1) != 63) {
            pos.x = ((uint16_t)pos.x >> 3) << 3;
            velocity.x = 0.0f;
        }
    }

    onGround = false;
    if(velocity.y <= 0.0f) {
        if(Level::getTileByPosition(pos.x + 0.0f, pos.y) != 63 || Level::getTileByPosition(pos.x + TILE_SIZE-1, pos.y) != 63) {
            pos.y = (((uint16_t)pos.y >> 3) + 1) << 3;
            velocity.y = 0.0f;
        }
    } else {
        if(Level::getTileByPosition(pos.x + 0.0f, pos.y + TILE_SIZE) != 63 || Level::getTileByPosition(pos.x + TILE_SIZE-1, pos.y + TILE_SIZE) != 63) {
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
    MovDir md = getMovingDirection();

    //this really should be handled by a sort of proper FSM
    //save on some duplicate checks
    if (mAnimState != AnimState::ATTACK && mAnimState != AnimState::IDLE && md == MovDir::NONE) {
        changeAnimation(AnimState::IDLE);
        return;
    }

    switch (mAnimState) {
    case AnimState::IDLE:
        if(md != MovDir::NONE) {
            if (md == MovDir::N) {
                changeAnimation(AnimState::JUMP);
            } else if (md == MovDir::S) {
                changeAnimation(AnimState::FALL);
            } else {
                changeAnimation(AnimState::WALK);
            }
        }
        break;
    case AnimState::WALK:
        if (md != MovDir::W && md != MovDir::E) {
            if (md == MovDir::N) {
                changeAnimation(AnimState::JUMP);
            } else {
                changeAnimation(AnimState::FALL);
            }
        }
        break;
    case AnimState::JUMP:
        if (md != MovDir::N) {
            if (md == MovDir::S) {
                changeAnimation(AnimState::FALL);
            } else {
                changeAnimation(AnimState::WALK);
            }
        }
        break;
    case AnimState::FALL:
        if (md != MovDir::N && md != MovDir::S) {
            changeAnimation(AnimState::WALK);
        }
        break;
    default:
        //execution shouldn't reach this
        break;
    }
}

void Player::changeAnimation(AnimState state) {
    switch (state) {
    case AnimState::IDLE:
        mAnimState = AnimState::IDLE;
        animFrameStart = ANIM_IDLE_START;
        animFramesNumber = ANIM_IDLE_FRAMES;
        break;
    case AnimState::WALK:
        mAnimState = AnimState::WALK;
        animFrameStart = ANIM_WALK_START;
        animFramesNumber = ANIM_WALK_FRAMES;
        break;
    case AnimState::JUMP:
        mAnimState = AnimState::JUMP;
        animFrameStart = ANIM_JUMP_START;
        animFramesNumber = ANIM_JUMP_FRAMES;
        break;
    case AnimState::FALL:
        mAnimState = AnimState::FALL;
        animFrameStart = ANIM_FALL_START;
        animFramesNumber = ANIM_FALL_FRAMES; 
        break;
    case AnimState::ATTACK:
        mAnimState = AnimState::ATTACK;
        animFrameStart = ANIM_ATTACK_START;
        animFramesNumber = ANIM_ATTACK_FRAMES;
        break;
    default:
        break;
    }
    //lets suppose default will not be reached and a state change is always guaranteed -> saves some repeating lines
    animFrameCurrent = 0;
    lastFrameUpdate = millis();
}