#ifndef ENEMY_H
#define ENEMY_H

#include "enemy_config.h"
#include "vec2.h"

#include <stdint.h>


class Graphics;

class Enemy {
public:
    Enemy();
    Enemy(const vec2& pos, const EnemyConfig::Config& config);
    ~Enemy();

    void update(float dt);
    void cleanPrevDraw(Graphics& graphics);
    void draw(Graphics& graphics);

    const vec2& getPos();
    EnemyConfig::Type getType();
    int8_t getDmg();

    bool hit(int8_t dmg,  int8_t force);

private:
    void checkCollision();

    vec2 mPos;
    vec2 mOldPos;
    vec2 mVelocity;

    EnemyConfig::Config mConfig;

    uint32_t mLastFrameUpdate = 0;
    uint32_t mLastSleepTime = 0; 

    uint8_t mAnimFrameCurrent = 0;
    
    bool mFlipSprite = false;
    bool mSleeps = false;
    bool mOnGround = false;

};

#endif // ENEMY_H