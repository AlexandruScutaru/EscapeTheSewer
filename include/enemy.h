#ifndef ENEMY_H
#define ENEMY_H

#include "vec2.h"

#include <stdint.h>


class Graphics;

enum class EnemyType : int8_t {
    NONE,
    SLIME,
    BUG,
};

class Enemy {
public:
    Enemy();
    Enemy(const vec2& pos, EnemyType type);
    ~Enemy();

    void update(float dt);
    void cleanPrevDraw(Graphics& graphics);
    void draw(Graphics& graphics);

    const vec2& getPos();
    EnemyType getType();
    bool hit(int8_t dmg,  int8_t force);

private:
    void checkCollision();

    vec2 mPos;
    vec2 mOldPos;
    vec2 mVelocity;

    float mWalkSpeed;
    float mJumpForce;
    float mGravity;

    uint32_t mLastFrameUpdate = 0;
    uint32_t mLastSleepTime = 0;
    
    EnemyType mType;

    uint8_t mAnimFrameCurrent = 0;
    uint8_t mAnimFrameStart;
    uint8_t mAnimFramesCount;
    uint8_t mAnimFrameTime;
    int8_t mHp;
    
    bool mFlipSprite = false;
    bool mSleeps = false;
    bool mOnGround = false;
    bool mCanJump = false;
    bool mCanSleep = false;

};

#endif // ENEMY_H