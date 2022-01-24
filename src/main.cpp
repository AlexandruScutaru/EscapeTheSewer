#include <Arduino.h>

#include "main_game.h"

void  setup() {
    Serial.begin(115200);
    MainGame game;
    game.run();
}

void loop() {}
