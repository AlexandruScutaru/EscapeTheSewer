#include <Arduino.h>

#if defined (WRITE_LEVELS)
#include "level_writer.h"
#else
#include "game_runner.h"
#endif

void  setup() {
    Serial.begin(115200);
    while (!Serial);
    Serial.println("setup");

#if defined(WRITE_LEVELS)
    WriteLevel();
#else
    GameRunner runner;
    runner.run();
#endif
}

void loop() {}
