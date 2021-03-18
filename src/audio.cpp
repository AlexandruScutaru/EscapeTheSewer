#include "audio.h"


#define BUZZER 7


#define NOTE_E4  330
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_A4  440
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_D5  587
#define NOTE_E5  659

#define E4  0
#define B4  1
#define C5  2
#define A4  3 
#define G4  4
#define FS4 5 
#define D5  6
#define E5  7


const int16_t Audio::notes[] = {NOTE_E4, NOTE_B4, NOTE_C5, NOTE_A4, NOTE_G4, NOTE_FS4, NOTE_D5, NOTE_E5};

const Audio::note_t Audio::melody[] /*PROGMEM*/ = {
    //NOTE_E4, NOTE_E4,

    {E4, 2}, {B4, 2}, {C5 , 2}, {E4, 2}, {A4 , 2}, {B4, 2}, {E4 , 2}, {G4, 2},
    {A4, 2}, {E4, 2}, {FS4, 2}, {G4, 2}, {E4 , 2}, {G4, 2}, {A4 , 4},
    {E4, 2}, {B4, 2}, {C5 , 2}, {E4, 2}, {A4 , 2}, {B4, 2}, {E4 , 2}, {G4, 2},
    {A4, 2}, {E4, 2}, {FS4, 2}, {G4, 2}, {A4 , 2}, {G4, 2}, {FS4, 4},
    {E4, 2}, {B4, 2}, {C5 , 2}, {E4, 2}, {A4 , 2}, {B4, 2}, {E4 , 2}, {G4, 2},
    {A4, 2}, {E4, 2}, {FS4, 2}, {G4, 2}, {E4 , 2}, {G4, 2}, {A4 , 4},
    {E4, 2}, {B4, 2}, {C5 , 2}, {E4, 2}, {A4 , 2}, {B4, 2}, {E4 , 2}, {G4, 2},
    {A4, 2}, {E4, 2}, {FS4, 2}, {G4, 2}, {FS4, 4}, {G4, 4},

    {E4, 2}, {B4, 2}, {C5 , 2}, {E4, 2}, {B4 , 2}, {C5, 2}, {E4 , 2}, {C5, 2},
    {D5, 2}, {E4, 2}, {D5 , 2}, {E5, 2}, {E4 , 2}, {B4, 2}, {C5 , 4},
    {E4, 2}, {B4, 2}, {C5 , 2}, {E4, 2}, {B4 , 2}, {C5, 2}, {E4 , 2}, {A4, 2},
    {B4, 2}, {E4, 2}, {G4 , 2}, {A4, 2}, {E4 , 2}, {A4, 2}, {B4 , 4},
    //
    //{E4, 2}, {B4, 2}, {C5 , 2}, {E4, 2}, {A4 , 2}, {B4, 2}, {E4 , 2}, {G4, 2},
    //{A4, 2}, {E4, 2}, {FS4, 2}, {G4, 2}, {E4 , 2}, {G4, 2}, {A4 , 4},
    //{E4, 2}, {B4, 2}, {C5 , 2}, {E4, 2}, {A4 , 2}, {B4, 2}, {E4 , 2}, {G4, 2},
    //{A4, 2}, {E4, 2}, {FS4, 2}, {G4, 2}, {A4 , 2}, {G4, 2}, {FS4, 4},
    //{E4, 2}, {B4, 2}, {C5 , 2}, {E4, 2}, {A4 , 2}, {B4, 2}, {E4 , 2}, {G4, 2},
    //{A4, 2}, {E4, 2}, {FS4, 2}, {G4, 2}, {E4 , 2}, {G4, 2}, {A4 , 4},
    //{E4, 2}, {B4, 2}, {C5 , 2}, {E4, 2}, {A4 , 2}, {B4, 2}, {E4 , 2}, {G4, 2},
    //{A4, 2}, {E4, 2}, {FS4, 2}, {G4, 2}, {FS4, 4}, {G4, 4},
    //
    //{0, 4},
};

uint8_t Audio::note;
uint8_t Audio::notesNb;
uint8_t Audio::subDivision;
const uint8_t Audio::msPerSixteenthNote = 60;

#define TEMPO_SCALE 1.5
const uint16_t Audio::t1_load = 0;
const uint16_t Audio::t1_count = (uint16_t)(3750 * TEMPO_SCALE);


void Audio::Init() {
    notesNb = sizeof(melody) / sizeof(note_t);

    //reset Timer1 Control Reg A
    TCCR1A = 0; 
    
    //set prescaler to 256
    TCCR1B |= (1 << CS12);
    TCCR1B &= ~(1 << CS11);
    TCCR1B &= ~(1 << CS10);

    //reset Timer1 and set compare value
    TCNT1 = t1_load;
    OCR1A = t1_count;

    //enable Timer1 compare instrerrupt
    TIMSK1 = (1 << OCIE1A);

    //enable global intrerrupts
    sei();
}

void Audio::OnTimerInterrupt() {
    //reset timer
    TCNT1 = t1_load;

    if (subDivision == melody[note].duration) {
        note = (note+1) % notesNb;
        subDivision = 0;
        int16_t noteDuration = melody[note].duration * msPerSixteenthNote * TEMPO_SCALE * 0.9;
        tone(BUZZER, notes[melody[note].frequency], noteDuration);
    }
    subDivision++;
}

//ISR(TIMER1_COMPA_vect) {
//    Audio::OnTimerInterrupt();
//}
