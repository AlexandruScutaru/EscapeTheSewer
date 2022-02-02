#pragma once

#include <stdint.h>

struct Level;

class LevelLoader {
public:
	bool loadLevel(uint8_t index, Level& level);
	bool writeCurrentLevelIndex();

};
