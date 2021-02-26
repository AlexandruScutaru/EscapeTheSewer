#ifndef MAIN_GAME_H
#define MAIN_GAME_H

#include "input_manager.h"
#include "graphics.h"


class MainGame {
public:
    MainGame();
    MainGame(const MainGame& other) = delete;
    ~MainGame();

    void run();

private:
    void init();
    void loop();
    void draw();
    void update(float dt);

    InputManager mInputManager;
    Graphics mGraphics;

    float posX = 0.0f;
    float posY = 0.0f;
    float oldPosX = -1.0f;
    float oldPosY = -1.0f;
    float velX = 0.0f;
    float velY = 0.0f;
    uint8_t animFrame = 0;
    bool onGround = false;
    bool flipSprite = false;

};

#endif //MAIN_GAME_H
