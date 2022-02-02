#ifndef LEVEL_LOADER
#define LEVEL_LOADER

#include <stdint.h>

struct Level;

class LevelLoader {
public:
    bool loadLevel(uint8_t index, Level& level);
    bool writeCurrentLevelIndex();

};

#endif // LEVEL_LOADER
