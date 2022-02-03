#ifndef ENEMEY_CONFIG
#define ENEMEY_CONFIG

#include <stdint.h>


namespace EnemyConfig {

    enum class Type : int8_t {
        SLIME = 0,
        BUG,
        NUM_TYPES,
    };

    struct Config {
        Config(float walkSpeed, float jumpFOrce, float gravity,
               uint8_t animStart, uint8_t animCount, uint8_t animTime,
               int8_t hp, int8_t damage, bool canJUmp, bool canSleep)
            : mWalkSpeed(walkSpeed)
            , mJumpForce(jumpFOrce)
            , mGravity(gravity)
            , mAnimFrameStart(animStart)
            , mAnimFramesCount(animCount)
            , mAnimFrameTime(animTime)
            , mHp(hp)
            , mDamage(damage)
        {
            mFlags.canJump = canJUmp;
            mFlags.canSleep = canSleep;
        }

        float mWalkSpeed;
        float mJumpForce;
        float mGravity;
        uint8_t mAnimFrameStart;
        uint8_t mAnimFramesCount;
        uint8_t mAnimFrameTime;
        int8_t mHp;
        int8_t mDamage;
        struct flags_t {
            uint8_t canJump   : 1;
            uint8_t canSleep  : 1;
        } mFlags;

    };

    const Config configs[static_cast<uint32_t>(Type::NUM_TYPES)] = {
        {  0.5f, 4.0f, 0.7f, 23, 4, 120, 10, 1,  true, false },   // SLIME
        { 0.35f, 0.0f, 1.2f, 27, 2, 150, 20, 2, false,  true },   // BUG
    };

}

#endif // ENEMEY_CONFIG
