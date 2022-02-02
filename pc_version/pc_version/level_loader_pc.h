#pragma once

#include <stdint.h>

struct Level;

namespace LevelLoader {
	bool LoadLevel(uint8_t index, Level& level);
	bool WriteCurrentLevelIndex();
};
