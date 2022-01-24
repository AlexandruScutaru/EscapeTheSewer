#include "enemy.h"
#include "level.h"
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

Enemy::Enemy(const vec2& mPos, const Config& config) 
    : mPos(mPos)
    , mOldPos(mPos)
    , mVelocity(vec2(0.0f))
    , mConfig(config)
{}

Enemy::~Enemy() {}


void Enemy::update(float dt) {
    mOldPos = mPos;

    if (mConfig.mCanSleep) {
        if(mLastSleepTime + 3000 <= millis()) {
            mAnimFrameCurrent = 2;
            mLastSleepTime = millis();
            mSleeps = true;
        }

        if(mSleeps  && mLastSleepTime + 1000 <= millis()) {
            mAnimFrameCurrent = 0;
            mLastFrameUpdate = millis();
            mSleeps = false;
        }
    }

    if (!mSleeps) {
        //I need to update this a mathematically smarter way to deal with impulses
        if (mFlipSprite) {
            if (mVelocity.x + mConfig.mWalkSpeed > 0.01f)
                mVelocity.x -= VELOCITY_RECOVERY * dt;
            else if (mVelocity.x + mConfig.mWalkSpeed < -0.01f)
                mVelocity.x += VELOCITY_RECOVERY * dt;
        } else {
            if (mVelocity.x - mConfig.mWalkSpeed > 0.01f)
                mVelocity.x -= VELOCITY_RECOVERY * dt;
            else if (mVelocity.x - mConfig.mWalkSpeed < -0.01f)
                mVelocity.x += VELOCITY_RECOVERY * dt;
        }

        mPos.x = min(max(0, mPos.x + mVelocity.x * dt), (Level::levelW << 3) - TILE_SIZE);
    }

    if (mConfig.mCanJump && mOnGround && mAnimFrameCurrent == 1) {
        mVelocity.y = -mConfig.mJumpForce;
        mOnGround = false;
    }

    mVelocity.y = min(2*mConfig.mGravity, mVelocity.y + mConfig.mGravity * dt);
    mPos.y += mVelocity.y * dt;
    if(mPos.y <= 0.0f) {
        mVelocity.y = 0.0f;
        mPos.y = 0.0f;
    }
    mPos.y = min(mPos.y, 120);

    checkCollision();
}

void Enemy::cleanPrevDraw(Graphics& graphics) {
    Level::cleanPrevDraw(mOldPos);
}

void Enemy::draw(Graphics& graphics) {
    if (mPos.x < graphics.camera.x1 << 3 || mPos.x + TILE_SIZE >= graphics.camera.x2 << 3)
        return;

    graphics.drawTile(mAnimFrameCurrent + mConfig.mAnimFrameStart, static_cast<uint16_t>(mPos.x), static_cast<uint16_t>(mPos.y), TILE_SIZE, mFlipSprite);

    if(mLastFrameUpdate + mConfig.mAnimFrameTime <= millis() && !mSleeps) {
        mAnimFrameCurrent = mAnimFrameCurrent + 1 == mConfig.mAnimFramesCount ? 0 : mAnimFrameCurrent + 1;
        mLastFrameUpdate = millis();
    }
}

const vec2& Enemy::getPos() {
    return mPos;
}

Type Enemy::getType() {
    return mConfig.mType;
}

int8_t Enemy::getDmg() {
    return mConfig.mDmg;
}

bool Enemy::hit(int8_t dmg, int8_t force) {
    if (mSleeps)
        return true;

    mVelocity.x = force;
    mVelocity.y -= 2.0f;
    mConfig.mHp -= dmg;

    return mConfig.mHp > 0;
}

void Enemy::checkCollision() {
    Level::collideWithLevel(mPos, mOldPos, mVelocity, vec2(mConfig.mWalkSpeed, 0.0f), &mFlipSprite, &mOnGround);
}
