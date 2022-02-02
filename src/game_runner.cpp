#include "game_runner.h"
#include "game.h"
#include "level.h"
#include "vec2.inl"

#if defined (ARDUINO) || defined (__AVR_ATmega328P__)
#include "level_loader.h"
#else
#include "../pc_version/pc_version/level_loader_pc.h"
#endif


void GameRunner::run() {
    Serial.println("run game");
    Level level;
    while (loadNextLevel(level)) {
        Serial.println("level loaded");
        Game game(level);
        game.run();
        mCurrentLevel++;
    }
    //done with all the levels, do some celebration or something if space allows
}

bool GameRunner::loadNextLevel(Level& level) {
    Serial.println("loadNextLevel");
    LevelLoader loader;
    return loader.loadLevel(mCurrentLevel, level);
}
