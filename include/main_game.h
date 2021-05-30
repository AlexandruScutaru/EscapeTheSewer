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
#include "status_bar.h"


class MainGame {
public:
    MainGame();
    MainGame(const MainGame& other) = delete;
    ~MainGame();

    void run();

private:
    enum class LevelState : int8_t {
        IN_PROGRESS,
        CLEARED,
        PLAYER_DIED,
    };

    void init();
    void loop();
    void draw();
    void update(float dt);

    LevelState mState = LevelState::IN_PROGRESS;

    InputManager mInputManager;
    Graphics mGraphics;
    Player mPlayer;
    StatusBar mStatusBar;

};

#endif //MAIN_GAME_H
