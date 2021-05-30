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
    void cleanPrevDraw(Graphics& graphics);
    void draw(Graphics& graphics);

    void setPos(const vec2& pos);
    vec2 getPos() const;
    int8_t getHp() const;

    bool hit(int8_t dmg);

private:
    enum class AnimState : int8_t {
        IDLE,
        WALK,
        JUMP,
        FALL,
        ATTACK,
        CLIMB_IDLE,
        CLIMBING,
    };
    enum class MovDir : int8_t {
        NONE,
        N,
        E,
        S,
        W
    };

    void checkCollision();
    void updateAnimation();
    void changeAnimation(AnimState state);
    MovDir getMovingDirection();

    vec2 pos;
    vec2 oldPos;
    vec2 velocity;

    uint32_t lastFrameUpdate = 0;
    uint16_t ladderXpos = 0;
    uint8_t animFrameStart = 0;
    uint8_t animFrameCurrent = 0;
    uint8_t animFramesNumber = 4;
    int8_t hp = 100;
    AnimState mAnimState = AnimState::WALK;
    MovDir mMovingDirection = MovDir::NONE;
    bool onGround = false;
    bool flipSprite = false;
    bool attackSuccesful = false;

};

#endif // PLAYER_H