#include <Arduino.h>

#if defined (WRITE_LEVELS)
#include "level_writer.h"
#else
#include "game.h"
#endif

void  setup() {
#if defined(SHOULD_LOG)
    Serial.begin(115200);
    while (!Serial);
#endif

#if defined(WRITE_LEVELS)
    WriteLevel();
#else
    Game game;
    game.run();
#endif
}

void loop() {}
