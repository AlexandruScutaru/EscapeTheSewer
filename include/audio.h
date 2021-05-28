#ifndef AUDIO_H
#define AUDIO_H

#include <Arduino.h>
#include <stdint.h>


class Audio {
public:
    static void Init();
    static void Disable();
    static void OnTimerInterrupt();

private:
    struct note_t {
        uint8_t frequency : 4;
        uint8_t duration  : 4;
    };

    Audio() {}

    static uint8_t note;
    static uint8_t notesNb;
    static uint8_t subDivision;
    const static uint8_t msPerSixteenthNote;
    const static int16_t notes[];
    const static note_t melody[] /*PROGMEM*/;

    const static uint16_t t1_load;
    const static uint16_t t1_count; 

};

#endif