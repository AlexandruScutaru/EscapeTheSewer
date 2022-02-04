#ifndef GAME_H
#define GAME_H

#include "level.h"
#include "player.h"
#include "status_bar.h"

#if defined (ARDUINO) || defined (__AVR_ATmega328P__)
    #include "input_manager.h"
    #include "graphics.h"
#else
    #include "../pc_version/pc_version/input_manager_pc.h"
    #include "../pc_version/pc_version/graphics_pc.h"
#endif


class Game {
public:
    Game();
    Game(const Game& other) = delete;
    ~Game();

    void run();

private:
    enum class LevelState : int8_t {
        IN_PROGRESS,
        FINISHED
    };

    void init();
    void loop();
    void draw();
    void update(float dt);
    void handleCollision();

    Level mLevel;
    InputManager mInputManager;
    Graphics mGraphics;
    Player mPlayer;
    StatusBar mStatusBar;

    LevelState mState = LevelState::IN_PROGRESS;
    uint8_t mCurrentLevel = 0;

};

#endif //GAME_H
