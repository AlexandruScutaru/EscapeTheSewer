#ifndef LEVEL_LOADER
#define LEVEL_LOADER

#include <FatReader.h>
#include "level.h"

class LevelLoader {
public:
    LevelLoader();
    ~LevelLoader();

    Level loadLevel(uint8_t index);
    bool writeCurrentLevelIndex();

private:
    SdReader card;
    FatVolume volume;
    FatReader rootDir;
    FatReader file;

};

#endif // LEVEL_LOADER
