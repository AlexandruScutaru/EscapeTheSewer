#ifndef LEVEL_LOADER
#define LEVEL_LOADER

#include <stdint.h>

struct Level;

namespace LevelLoader {
    bool LoadLevel(uint8_t index, Level& level);
};

#endif // LEVEL_LOADER
