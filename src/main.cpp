#include <Arduino.h>

#if defined (WRITE_LEVELS)
#include "level_writer.h"
#else
#include "game.h"
#include <avr/io.h>
#endif

void initSpi() {
    // backup sreg and disable interrupts maybe?
    //uint8_t sreg = SREG;
    //cli();

    // Set MOSI and SCK, CS output, all others input
    DDRB = _BV(PB3) | _BV(PB5) | _BV(PB2);
    // Enable SPI, Master, set clock rate fck/4, mode 0
    SPCR = _BV(SPE) | _BV(MSTR);

    // Set CS
    PORTB |= _BV(PB2);

    // restore sreg
    //SREG = sreg;
}

void  setup() {
#if defined(SHOULD_LOG)
    Serial.begin(115200);
    while (!Serial);
#endif

#if defined(WRITE_LEVELS)
    WriteLevel();
#else
    initSpi();
    Game game;
    game.run();
#endif
}

void loop() {}
