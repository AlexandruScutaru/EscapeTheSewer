#include <Arduino.h>

#include "game_runner.h"

void  setup() {
    //Serial.begin(115200);
    GameRunner runner;
    runner.run();
}

void loop() {}
