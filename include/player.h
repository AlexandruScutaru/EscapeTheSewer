#ifndef PLAYER_H
#define PLAYER_H

#include "vector2.h"

#include <stdint.h>


class InputManager;
class Graphics;

class Player {
public:
    Player();
    ~Player();

    void update(InputManager& input, float dt);
    void draw(Graphics& graphics);

private:
    enum class AnimState {
        IDLE,
        WALK,
        JUMP,
        FALL,
        ATTACK
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

    Vector2 pos;
    Vector2 oldPos;
    Vector2 velocity;
    bool onGround = false;
    bool flipSprite = false;
    AnimState mAnimState = AnimState::WALK;
    MovDir mMovingDirection = MovDir::NONE;
    uint8_t animFrameStart = 0;
    uint8_t animFrameCurrent = 0;
    uint8_t animFramesNumber = 4;
    uint8_t ticksPerAnimFrame = 0;

};

#endif // PLAYER_H