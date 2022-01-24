#include "enemy_config.h"


namespace EnemyConfig {

    Config::Config(Type type, float walkSpeed, float jumpForce, float gravity, 
                   uint8_t animStart, uint8_t animCount, uint8_t animTime,
                   int8_t hp, int8_t dmg, bool canJump, bool canSleep)
        : mType(type)       
        , mWalkSpeed(walkSpeed)
        , mJumpForce(jumpForce)
        , mGravity(gravity)
        , mAnimFrameStart(animStart)
        , mAnimFramesCount(animCount)
        , mAnimFrameTime(animTime)
        , mHp(hp)
        , mDmg(dmg)
        , mCanJump(canJump)
        , mCanSleep(canSleep)
    {}

    Config GetConfig(Type type) {
        switch (type) {
            case Type::SLIME:
                return Config(type, 0.5f, 4.0f, 0.7f, 23, 4, 120, 10, 1, true, false);
            case Type::BUG:
                return Config(type, 0.35f, 0.0f, 1.2f, 27, 2, 150, 20, 2, false, true);
            case Type::UNKNOWN:
            default:
                return Config();
        }
    }
}
