#include "game_runner.h"
#include "game.h"
#include "level.h"
#include "level_loader.h"


void GameRunner::run() {
    Serial.println("runner::run");
    while (auto level = loadNextLevel()) {
        Game game(level);
        game.run();
        mCurrentLevel++;
    }
    //done with all the levels
}

Level GameRunner::loadNextLevel() {
    LevelLoader loader;
    return loader.loadLevel(mCurrentLevel);
}
