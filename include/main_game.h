#ifndef MAIN_GAME_H
#define MAIN_GAME_H

#if defined (ARDUINO) || defined (__AVR_ATmega328P__)
    #include "input_manager.h"
    #include "graphics.h"
#else
    #include "../pc_version/input_manager_pc.h"
    #include "../pc_version/graphics_pc.h"
#endif

#include "player.h"
#include "slime.h"
#include "coin.h"


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

    //these can be updated to have a common interface but I'm not sure I want to make them polymorphic and add vtable overhead on arduino...
    //maybe some experiments with CRTP? or plain inheritance and non virtual overriding? mneah
    Player mPlayer;
    Slime mSlime;
    Coin mCoin;

};

#endif //MAIN_GAME_H
