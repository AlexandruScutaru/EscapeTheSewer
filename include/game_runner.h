#ifndef GAME_RUNNNER_H
#define GAME_RUNNNER_H

#include <stdint.h>

struct Level;

class GameRunner {
public:
    void run();

private:
    Level loadNextLevel();
    uint8_t mCurrentLevel = 0;
};

#endif // GAME_RUNNNER_H
