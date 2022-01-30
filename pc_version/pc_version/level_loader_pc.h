#pragma once

#include "level.h"

class LevelLoader {
public:
	Level loadLevel(uint8_t index);
	bool writeCurrentLevelIndex();

};
