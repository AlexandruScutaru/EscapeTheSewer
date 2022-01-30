#include "game_runner.h"
#include "game.h"
#include "level.h"

#if defined (ARDUINO) || defined (__AVR_ATmega328P__)
#include "level_loader.h"
#else
#include "../pc_version/pc_version/level_loader_pc.h"
#endif


void GameRunner::run() {
    while (auto level = loadNextLevel()) {
        Game game(level);
        game.run();
        mCurrentLevel++;
    }
    //done with all the levels, do some celebration or something if space allows
}

Level GameRunner::loadNextLevel() {
    LevelLoader loader;
    return loader.loadLevel(mCurrentLevel);
}
