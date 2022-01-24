#ifndef LEVEL_LOADER
#define LEVEL_LOADER

#include <FatReader.h>


class LevelLoader {
public:
    LevelLoader();
    ~LevelLoader();

    bool loadNextLevel();
    bool writeCurrentLevelIndex();

private:
    SdReader card;
    FatVolume volume;
    FatReader rootDir;
    FatReader file;

};

#endif // LEVEL_LOADER
