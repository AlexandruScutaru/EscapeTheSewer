#ifndef ENEMEY_CONFIG
#define ENEMEY_CONFIG

#include <stdint.h>


namespace EnemyConfig {

    enum class Type : int8_t {
        SLIME,
        BUG,
        UNKNOWN,
    };

    struct Config {
        Config(Type type = Type::UNKNOWN, float walkSpeed = 0.0f, float jumpFOrce = 0.0f, float gravity = 0.0f,
               uint8_t animStart = 0, uint8_t animCount = 0, uint8_t animTime = 0,
               int8_t hp = 0, int8_t dmg = 0, bool canJUmp = false, bool canSleep = false);

        Type mType;
        float mWalkSpeed;
        float mJumpForce;
        float mGravity;
        uint8_t mAnimFrameStart;
        uint8_t mAnimFramesCount;
        uint8_t mAnimFrameTime;
        int8_t mHp;
        int8_t mDmg;
        bool mCanJump;
        bool mCanSleep;
    };

    Config GetConfig(Type type);

}

#endif // ENEMEY_CONFIG
