#include "player.h"
#include "level_utils.h"
#include "vec2.inl"

#if defined (ARDUINO) || defined (__AVR_ATmega328P__)
    #include "input_manager.h"
    #include "graphics.h"
    #include <Arduino.h>

    #define LOG(msg)
#else
    #include "../pc_version/pc_version/input_manager_pc.h"
    #include "../pc_version/pc_version/graphics_pc.h"
    #include "../pc_version/pc_version/logging.h"

    #define min(a,b) ((a)<(b)?(a):(b))
    #define max(a,b) ((a)>(b)?(a):(b))

    #define millis() Graphics::getElapsedTime()
#endif

#define WALK_SPEED       1.2f
#define CLIMB_SPEED      0.7f
#define GRAVITY          0.9f
#define JUMP_FORCE       7.0f

#define ANIM_WALK_START   32
#define ANIM_WALK_FRAMES   4
#define ANIM_IDLE_START   36
#define ANIM_IDLE_FRAMES   4
#define ANIM_FALL_START   40
#define ANIM_FALL_FRAMES   2
#define ANIM_JUMP_START   42
#define ANIM_JUMP_FRAMES   2
#define ANIM_CLIMB_START  44
#define ANIM_CLIMB_FRAMES  2
#define ANIM_ATTACK_START 46
#define ANIM_ATTACK_FRAMES 2

#define ANIM_FRAME_TIME  100

#define SWORD_REACH 3.0f
#define SWORD_DMG 3.0f
#define SWORD_KNOCKBACK_FORCE 2.0f


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
    oldPos = pos;
}

vec2 Player::getPos() const {
    return pos;
}

int8_t Player::getHp() const {
    return hp;
}

void Player::incHp(uint8_t val) {
    hp = min(100, hp + val);
}

bool Player::hit(int8_t dmg) {
    hp -= dmg;
    return hp > 0;
}

void Player::update(InputManager& input, Level& level, Graphics& graphics, float dt) {
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

    if (input.wasButtonPressedNow(InputManager::Button::B) && (mAnimState == AnimState::IDLE || mAnimState == AnimState::WALK || mAnimState == AnimState::JUMP || mAnimState == AnimState::FALL)) {
        changeAnimation(AnimState::ATTACK);
        attackSuccesful = false;
    }

    if(mAnimState == AnimState::CLIMBING || mAnimState == AnimState::CLIMB_IDLE) {
        if (input.isButtonPressed(InputManager::Button::UP)) {
            velocity.y = -CLIMB_SPEED;
            pos.x = ladderXpos;
        } else if (input.isButtonPressed(InputManager::Button::DOWN)) {
            velocity.y = CLIMB_SPEED;
            pos.x = ladderXpos;
        } else {
            velocity.y = 0.0f;
        }
    } else {
        velocity.y = min(2*GRAVITY, velocity.y + GRAVITY * dt);
    }

    //pos.x = min(max(Graphics::camera.x1 << 3, pos.x + velocity.x * dt), (Graphics::camera.x2 << 3) - TILE_SIZE);
    pos.x = min(max(0, pos.x + velocity.x * dt), (level.levelW << 3) - TILE_SIZE);
    pos.y += velocity.y * dt;
    if(pos.y <= 0.0f) {
        velocity.y = 0.0f;
        pos.y = 0.0f;
    }
    pos.y = min(pos.y, (level.levelH << 3) - TILE_SIZE);

    checkCollision(level, graphics);
    updateAnimation();
}

void Player::cleanPrevDraw(Level& level, Graphics& graphics) {
    LevelUtils::cleanPrevDraw(level, graphics, oldPos);
}

void Player::draw(Graphics& graphics) {
    if ((Graphics::camera.x2 << 3) - pos.x <= 5*TILE_SIZE) {
        graphics.scroll(true);
    } else if (pos.x - (Graphics::camera.x1 << 3) <= 5*TILE_SIZE) {
        graphics.scroll(false);
    }

    graphics.drawTile(animFrameCurrent + animFrameStart, static_cast<uint16_t>(pos.x), static_cast<uint16_t>(pos.y), TILE_SIZE, flipSprite);
    //add proper Timer implementation to be reused across app, with a callback as to avoid constant polling
    if(lastFrameUpdate + ANIM_FRAME_TIME <= millis()) {
        //jeezuhs.. damn this animation handling is bad
        //can't call it code smell.. it is an overwhelming stench
        if (mAnimState == AnimState::ATTACK && animFrameCurrent == 1) {
            changeAnimation(AnimState::IDLE);
        } else {
            if(mAnimState == AnimState::CLIMB_IDLE)
                animFrameCurrent = 0;
            else
                animFrameCurrent = animFrameCurrent + 1 == animFramesNumber ? 0 : animFrameCurrent + 1;
        }
        lastFrameUpdate = millis();
    }
}

void Player::checkCollision(Level& level, Graphics& graphics) {
    bool collidedWithLadder = LevelUtils::collideWithLevel(level, pos, oldPos, velocity, vec2(0.0f), nullptr, &onGround, &ladderXpos);

    if (collidedWithLadder) {
        if(mAnimState != AnimState::CLIMBING && mAnimState != AnimState::CLIMB_IDLE) {
            changeAnimation(AnimState::CLIMB_IDLE);
        }
    } else if(mAnimState == AnimState::CLIMBING || mAnimState == AnimState::CLIMB_IDLE) {
        changeAnimation(AnimState::IDLE);
    }

    if (mAnimState == AnimState::ATTACK && !attackSuccesful) 
    {
        attackSuccesful = LevelUtils::hitEnemy(level, graphics, vec2(flipSprite ? pos.x - SWORD_REACH : pos.x + SWORD_REACH, pos.y), SWORD_DMG, flipSprite ? -SWORD_KNOCKBACK_FORCE : SWORD_KNOCKBACK_FORCE);
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
    if (md == MovDir::NONE && (mAnimState == AnimState::WALK || mAnimState == AnimState::JUMP || mAnimState == AnimState::FALL)) {
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
    case AnimState::CLIMB_IDLE:
        if(md == MovDir::N || md == MovDir::S) {
            changeAnimation(AnimState::CLIMBING);
        }
        break;
    case AnimState::CLIMBING:
        if(md != MovDir::N && md != MovDir::S) {
            changeAnimation(AnimState::CLIMB_IDLE);
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
        animFrameStart = ANIM_IDLE_START;
        animFramesNumber = ANIM_IDLE_FRAMES;
        break;
    case AnimState::WALK:
        animFrameStart = ANIM_WALK_START;
        animFramesNumber = ANIM_WALK_FRAMES;
        break;
    case AnimState::JUMP:
        animFrameStart = ANIM_JUMP_START;
        animFramesNumber = ANIM_JUMP_FRAMES;
        break;
    case AnimState::FALL:
        animFrameStart = ANIM_FALL_START;
        animFramesNumber = ANIM_FALL_FRAMES; 
        break;
    case AnimState::ATTACK:
        animFrameStart = ANIM_ATTACK_START;
        animFramesNumber = ANIM_ATTACK_FRAMES;
        break;
    case AnimState::CLIMBING:
    case AnimState::CLIMB_IDLE:
        animFrameStart = ANIM_CLIMB_START;
        animFramesNumber = ANIM_CLIMB_FRAMES;
        break;
    default:
        break;
    }
    //lets suppose default will not be reached and a state change is always guaranteed -> saves some repeating lines
    mAnimState = state;
    animFrameCurrent = 0;
    lastFrameUpdate = millis();
}
