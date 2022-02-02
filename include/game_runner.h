#ifndef GAME_RUNNNER_H
#define GAME_RUNNNER_H

#include <stdint.h>

struct Level;

class GameRunner {
public:
    void run();

private:
    bool loadNextLevel(Level& level);
    uint8_t mCurrentLevel = 0;
};

#endif // GAME_RUNNNER_H
