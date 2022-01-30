#ifndef ENEMY_H
#define ENEMY_H

#include "enemy_config.h"
#include "vec2.h"

#include <stdint.h>


class Graphics;
class Level;

class Enemy {
public:
    Enemy();
    Enemy(const vec2& pos, EnemyConfig::Type type);
    ~Enemy();

    void update(Level& level, Graphics& graphics, float dt);
    void cleanPrevDraw(Level& level, Graphics& graphics);
    void draw(Graphics& graphics);

    const vec2& getPos();
    EnemyConfig::Type getType();
    int8_t getDmg();

    bool hit(int8_t dmg,  int8_t force);

private:
    void checkCollision(Level& level);

    vec2 mPos;
    vec2 mOldPos;
    vec2 mVelocity;

    uint32_t mLastFrameUpdate = 0;
    uint32_t mLastSleepTime = 0; 

    uint8_t mAnimFrameCurrent = 0;
    uint8_t mConfigIndex = 0;
    int8_t mHealth = 0;

    struct flags_t {
        uint8_t flipSprite  : 1;
        uint8_t sleeps      : 1;
        uint8_t onGround    : 1;
    } mFlags;

};

#endif // ENEMY_H