#include "enemy.h"
#include "level_utils.h"
#include "vec2.inl"

#if defined (ARDUINO) || defined (__AVR_ATmega328P__)
    #include "graphics.h"
    #include <Arduino.h>
#else
    #include "../pc_version/pc_version/graphics_pc.h"
    #include "../pc_version/pc_version/logging.h"

    #define min(a,b) ((a)<(b)?(a):(b))
    #define max(a,b) ((a)>(b)?(a):(b))

    #define millis() Graphics::getElapsedTime()
#endif

#define VELOCITY_RECOVERY 0.2f

using namespace EnemyConfig;


Enemy::Enemy() {}

Enemy::Enemy(const vec2& mPos, Type type)
    : mPos(mPos)
    , mOldPos(mPos)
    , mVelocity(vec2(0.0f))
    , mConfigIndex(static_cast<uint8_t>(type))
{
    mHealth = configs[mConfigIndex].mHp;
    mFlags.flipSprite = false;
    mFlags.sleeps = false;
    mFlags.onGround = false;
}

Enemy::~Enemy() {}


void Enemy::update(Level& level, Graphics& graphics, float dt) {
    mOldPos = mPos;

    if (configs[mConfigIndex].mFlags.canSleep) {
        if(mLastSleepTime + 3000 <= millis()) {
            mAnimFrameCurrent = 2;
            mLastSleepTime = millis();
            mFlags.sleeps = true;
        }

        if(mFlags.sleeps  && mLastSleepTime + 1000 <= millis()) {
            mAnimFrameCurrent = 0;
            mLastFrameUpdate = millis();
            mFlags.sleeps = false;
        }
    }

    if (!mFlags.sleeps) {
        //I need to update this a mathematically smarter way to deal with impulses
        if (mFlags.flipSprite) {
            if (mVelocity.x + configs[mConfigIndex].mWalkSpeed > 0.01f)
                mVelocity.x -= VELOCITY_RECOVERY * dt;
            else if (mVelocity.x + configs[mConfigIndex].mWalkSpeed < -0.01f)
                mVelocity.x += VELOCITY_RECOVERY * dt;
        } else {
            if (mVelocity.x - configs[mConfigIndex].mWalkSpeed > 0.01f)
                mVelocity.x -= VELOCITY_RECOVERY * dt;
            else if (mVelocity.x - configs[mConfigIndex].mWalkSpeed < -0.01f)
                mVelocity.x += VELOCITY_RECOVERY * dt;
        }

        mPos.x = min(max(0, mPos.x + mVelocity.x * dt), (level.levelW << 3) - TILE_SIZE);
    }

    if (configs[mConfigIndex].mFlags.canJump && mFlags.onGround && mAnimFrameCurrent == 1) {
        mVelocity.y = -configs[mConfigIndex].mJumpForce;
        mFlags.onGround = false;
    }

    mVelocity.y = min(2 * configs[mConfigIndex].mGravity, mVelocity.y + configs[mConfigIndex].mGravity * dt);
    mPos.y += mVelocity.y * dt;
    if(mPos.y <= 0.0f) {
        mVelocity.y = 0.0f;
        mPos.y = 0.0f;
    }
    mPos.y = min(mPos.y, 120);

    checkCollision(level);
}

void Enemy::cleanPrevDraw(Level& level, Graphics& graphics) {
    LevelUtils::cleanPrevDraw(level, graphics, mOldPos);
}

void Enemy::draw(Graphics& graphics) {
    if (mPos.x < graphics.camera.x1 << 3 || mPos.x + TILE_SIZE >= graphics.camera.x2 << 3)
        return;

    graphics.drawTile(mAnimFrameCurrent + configs[mConfigIndex].mAnimFrameStart, static_cast<uint16_t>(mPos.x), static_cast<uint16_t>(mPos.y), TILE_SIZE, mFlags.flipSprite);

    if(mLastFrameUpdate + configs[mConfigIndex].mAnimFrameTime <= millis() && !mFlags.sleeps) {
        mAnimFrameCurrent = mAnimFrameCurrent + 1 == configs[mConfigIndex].mAnimFramesCount ? 0 : mAnimFrameCurrent + 1;
        mLastFrameUpdate = millis();
    }
}

const vec2& Enemy::getPos() {
    return mPos;
}

Type Enemy::getType() {
    return static_cast<Type>(mConfigIndex);
}

int8_t Enemy::getDmg() {
    return configs[mConfigIndex].mDamage;
}

bool Enemy::hit(int8_t dmg, int8_t force) {
    if (mFlags.sleeps)
        return true;

    mVelocity.x = force;
    mVelocity.y -= 2.0f;
    mHealth -= dmg;

    return configs[mConfigIndex].mHp > 0;
}

void Enemy::checkCollision(Level& level) {
    //not looking nice, but at least the memory footprint is reduced if adding more flags
    bool flip = mFlags.flipSprite;
    bool onGround = mFlags.onGround;
    LevelUtils::collideWithLevel(level, mPos, mOldPos, mVelocity, vec2(configs[mConfigIndex].mWalkSpeed, 0.0f), &flip, &onGround);
    mFlags.flipSprite = flip;
    mFlags.onGround = onGround;
}
