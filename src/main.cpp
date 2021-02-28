#include <Arduino.h>

#include "main_game.h"


void  setup() {
    Serial.begin(9600);
    MainGame game;
    game.run();
}

void loop() {

}
