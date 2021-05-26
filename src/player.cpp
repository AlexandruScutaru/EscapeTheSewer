#include "player.h"
#include "level.h"
#include "vec2.inl"

#if defined (ARDUINO) || defined (__AVR_ATmega328P__)
    #include "input_manager.h"
    #include "graphics.h"
    #include <Arduino.h>

    #define LOG(msg)
#else
    #include "../pc_version/input_manager_pc.h"
    #include "../pc_version/graphics_pc.h"
    #include "../pc_version/logging.h"

    #define min(a,b) ((a)<(b)?(a):(b))
    #define max(a,b) ((a)>(b)?(a):(b))

    #define millis() Graphics::getElapsedTime()
#endif

#define WALK_SPEED       1.2f
#define CLIMB_SPEED      0.7f
#define GRAVITY          0.9f
#define JUMP_FORCE       7.0f

#define ANIM_FRAME_TIME  100

#define SWORD_REACH 3
#define SWORD_DMG 3
#define SWORD_KNOCKBACK_FORCE 2

#define IF_NON_COLLIDABLE(tile) if (tile.tile_index.index >= TILE_NON_COLLIDABLE_THRESHOLD)


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
    pos.x = min(max(0, pos.x + velocity.x * dt), (Level::levelW << 3) - TILE_SIZE);
    pos.y += velocity.y * dt;
    if(pos.y <= 0.0f) {
        velocity.y = 0.0f;
        pos.y = 0.0f;
    }
    pos.y = min(pos.y, (Level::levelH << 3) - TILE_SIZE);

    checkCollision();
    updateAnimation();
}

void Player::cleanPrevDraw(Graphics& graphics) {
    //clean up (redraw) the 4 adjacent tiles behind last drawn position
    //find maybe a way to draw when it is actually needed, as sending data to LCD to be displayed is very costly
    //TODO: 
    //  do the checks based on velocity, maybe something can come out of that
    uint8_t iCoord = (uint8_t)oldPos.y >> 3;
    uint8_t jCoord = (uint8_t)oldPos.x >> 3;

    Level::tile_index_t tileIndex = Level::getTileByIndex(iCoord, jCoord);
    IF_NON_COLLIDABLE(tileIndex) graphics.drawTile(tileIndex.tile_index.index, jCoord << 3, iCoord << 3, TILE_SIZE, tileIndex.tile_index.flip);

    if (iCoord + 1 < Level::levelH) {
        tileIndex = Level::getTileByIndex(iCoord+1, jCoord);
        IF_NON_COLLIDABLE(tileIndex) graphics.drawTile(tileIndex.tile_index.index, jCoord << 3, (iCoord+1) << 3, TILE_SIZE, tileIndex.tile_index.flip);

        if(jCoord + 1 < Level::levelW) {
            tileIndex = Level::getTileByIndex(iCoord, jCoord + 1);
            IF_NON_COLLIDABLE(tileIndex) graphics.drawTile(tileIndex.tile_index.index, (jCoord+1) << 3, iCoord << 3, TILE_SIZE, tileIndex.tile_index.flip);
            //both above are true, directly draw diagonaly too
            tileIndex = Level::getTileByIndex(iCoord + 1, jCoord + 1);
            IF_NON_COLLIDABLE(tileIndex) graphics.drawTile(tileIndex.tile_index.index, (jCoord+1) << 3, (iCoord+1) << 3, TILE_SIZE, tileIndex.tile_index.flip);
        }
    }else if(jCoord + 1 < Level::levelW) {
        tileIndex = Level::getTileByIndex(iCoord, jCoord + 1);
        IF_NON_COLLIDABLE(tileIndex) graphics.drawTile(tileIndex.tile_index.index, (jCoord+1) << 3, iCoord << 3, TILE_SIZE, tileIndex.tile_index.flip);        
    }
}

void Player::draw(Graphics& graphics) {
    if ((Graphics::camera.x2 << 3) - pos.x <= 5*TILE_SIZE) {
        if (graphics.scroll(true)) {
            //pos.x -= TILE_SIZE;
        }
    } else if (pos.x - (Graphics::camera.x1 << 3) <= 5*TILE_SIZE) {
        if (graphics.scroll(false)) {
            //pos.x += TILE_SIZE;
        }
    }

    graphics.drawTile(animFrameCurrent + animFrameStart, pos.x, pos.y, TILE_SIZE, flipSprite);
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

void Player::checkCollision() {
    //if(velocity == vec2(0.0f))
    //    return;
    uint8_t leftUp    = Level::getTileByPosition(pos.x + 0.0f, oldPos.y + 0.0f);
    uint8_t leftDown  = Level::getTileByPosition(pos.x + 0.0f, oldPos.y + TILE_SIZE-1);
    uint8_t rightUp   = Level::getTileByPosition(pos.x + TILE_SIZE, oldPos.y + 0.0f);
    uint8_t rightDown = Level::getTileByPosition(pos.x + TILE_SIZE, oldPos.y + TILE_SIZE-1);
    if(velocity.x <= 0.0f) {
        if(leftUp < TILE_NON_COLLIDABLE_THRESHOLD || leftDown < TILE_NON_COLLIDABLE_THRESHOLD) {
            pos.x = (((uint16_t)pos.x >> 3) + 1) << 3;
            velocity.x = 0.0f;
        }
    } else {
        if(rightUp < TILE_NON_COLLIDABLE_THRESHOLD || rightDown < TILE_NON_COLLIDABLE_THRESHOLD) {
            pos.x = ((uint16_t)pos.x >> 3) << 3;
            velocity.x = 0.0f;
        }
    }

    onGround = false;
    uint8_t upLeft    = Level::getTileByPosition(pos.x + 0.0f, pos.y);
    uint8_t upRight   = Level::getTileByPosition(pos.x + TILE_SIZE-1, pos.y);
    uint8_t downLeft  = Level::getTileByPosition(pos.x + 0.0f, pos.y + TILE_SIZE);
    uint8_t downRight = Level::getTileByPosition(pos.x + TILE_SIZE-1, pos.y + TILE_SIZE);
    if(velocity.y <= 0.0f) {
        if (upLeft < TILE_NON_COLLIDABLE_THRESHOLD || upRight < TILE_NON_COLLIDABLE_THRESHOLD) {
            pos.y = (((uint16_t)pos.y >> 3) + 1) << 3;
            velocity.y = 0.0f;
        }
    } else {
        if (downLeft < TILE_NON_COLLIDABLE_THRESHOLD || downRight < TILE_NON_COLLIDABLE_THRESHOLD) {
            pos.y = min(((uint16_t)pos.y >> 3) << 3, 120);
            velocity.y = 0.0f;
            onGround = true;
        }
    }

    if (leftUp    == TILE_LADDER ||
        leftDown  == TILE_LADDER ||
        rightUp   == TILE_LADDER ||
        rightDown == TILE_LADDER ||
        upLeft    == TILE_LADDER ||
        upRight   == TILE_LADDER ||
        downLeft  == TILE_LADDER ||
        downRight == TILE_LADDER)
    {
        if(mAnimState != AnimState::CLIMBING && mAnimState != AnimState::CLIMB_IDLE) {
            changeAnimation(AnimState::CLIMB_IDLE);
        }
        if (leftUp == TILE_LADDER || leftDown == TILE_LADDER || upLeft == TILE_LADDER || downLeft == TILE_LADDER)
            ladderXpos = (((uint16_t)pos.x >> 3) + 0) << 3;
        else
            ladderXpos = (((uint16_t)pos.x >> 3) + 1) << 3;
    } else if(mAnimState == AnimState::CLIMBING || mAnimState == AnimState::CLIMB_IDLE) {
        changeAnimation(AnimState::IDLE);
    }

    size_t idx = 0;
    if (mAnimState == AnimState::ATTACK && !attackSuccesful && 
        Level::EntityType::SLIME == Level::getCollidedEntity(vec2(flipSprite ? pos.x - SWORD_REACH : pos.x + SWORD_REACH, pos.y), idx)) 
    {
        Level::hitEntity(Level::EntityType::SLIME, idx, SWORD_DMG, flipSprite ? -SWORD_KNOCKBACK_FORCE : SWORD_KNOCKBACK_FORCE);
        attackSuccesful = true;
    }

    Level::EntityType entt = Level::getCollidedEntity(pos, idx);
    switch (entt) {
    case Level::EntityType::NONE:
        break;
    case Level::EntityType::SLIME:
        //LOG("you got hit by a slime");
        break;
    case Level::EntityType::COIN:
        Level::removeEntity(entt, idx);
        break;
    case Level::EntityType::END:
        break;
    default:
        break;
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