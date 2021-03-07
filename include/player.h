#ifndef PLAYER_H
#define PLAYER_H

#include "vec2.h"

#include <stdint.h>


class InputManager;
class Graphics;

class Player {
public:
    Player();
    Player(float x, float y);
    ~Player();

    void update(InputManager& input, float dt);
    void draw(Graphics& graphics);

    void setPos(const vec2& pos);

private:
    enum class AnimState {
        IDLE,
        WALK,
        JUMP,
        FALL,
        ATTACK,
        CLIMB_IDLE,
        CLIMBING,
    };
    enum class MovDir {
        NONE,
        N,
        E,
        S,
        W
    };

    void checkCollision();
    MovDir getMovingDirection();
    void updateAnimation();
    void changeAnimation(AnimState state);

    vec2 pos;
    vec2 oldPos;
    vec2 velocity;
    bool onGround = false;
    bool flipSprite = false;
    bool attackSuccesful = false;
    uint16_t ladderXpos = 0;
    AnimState mAnimState = AnimState::WALK;
    MovDir mMovingDirection = MovDir::NONE;
    uint8_t animFrameStart = 0;
    uint8_t animFrameCurrent = 0;
    uint8_t animFramesNumber = 4;
    uint32_t lastFrameUpdate = 0;

};

#endif // PLAYER_H