# EscapeTheSewer
## Simple side-scrolling platformer game for an Arduino handheld console.

This [youtube video](https://www.youtube.com/watch?v=chjQD4TIpNc) shows a rather poor recording of it running on the Arduino.

Below is a sample of it running on pc.
<img src="https://github.com/AlexandruScutaru/EscapeTheSewer/blob/master/readme_resources/EscapeTheSewer.gif" alt="EscapeTheSewer.gif" width="60%"/>

# Software
## Graphics
### How would on get tiles to be drawn on the display?

The first thing that would seem fine to do is to save these tiles on a microSD card and just read the tiles whenever needed and display them (even go to lengths and cache some of those?).\
Yeah, it works fine but it is so slow that you can see the pixels coming in, that is not an experience you can build a game on.

Next logical thing would to store the tiles on Arduino's memory and read them directly from there, right? NO!\
Arduino Nano has a flash of 32KB and the tileset I ended up using is around 12KB. That would waste almost half the space. The rest needs to store all the code that drives the game, the LCD driver, the bootloader, the melodies and some level data.

I then thought about defining a limited color pallete. I'm not that good of an artist so I searched a bit through [itch.io](https://itch.io/) and I stumbled across [this tileset](https://0x72.itch.io/8x8-f24-tileset) which is based on a 24 colors pallete.\
As a programmer `24` wasn't going to cut for my needs to decrease memory footprint so I resorted to the next smaller power of 2, and that is `16` colors. I extracted 16 colors from the pallete and redraw the tileset with those.

### What is a `tile` for Arduino?
I will walkthrough how did I get from this bmp tileset <img src=https://github.com/AlexandruScutaru/EscapeTheSewer/blob/master/resources/f24.bmp alt="f24.bmp" width="30%"/> to single tiles being displayed by Arduino on the LCD.

The color pallete has 16 colors, this means there are required 4 bits to index any color.
The tiles used in this project are all 8x8. So there are 8x8x4 bits required to define a tile. This adds up to a total of 32 bytes per tile.
Following this, a bitfield struct of 4 bytes in size can be declared to store a tile row. 8 of those to define the entire tile.

```cpp
struct tile_row_t {
    uint32_t col1 : 4;
    uint32_t col2 : 4;
    uint32_t col3 : 4;
    uint32_t col4 : 4;
    uint32_t col5 : 4;
    uint32_t col6 : 4;
    uint32_t col7 : 4;
    uint32_t col8 : 4;
};

typedef tile_row_t tile_t[8];
```
The tileset is limited to 64 tiles this results in 2 kilobytes to store the entire tileset and those are defined like this:
```cpp
const tile_t tiles[] PROGMEM = {
    {
        {2, 3, 2, 2, 1, 0, 0, 0},
        {4, 4, 3, 1, 1, 1, 0, 2},
        {4, 3, 2, 3, 1, 1, 0, 0},
        {2, 3, 2, 2, 1, 0, 0, 0},
        {4, 3, 2, 2, 1, 0, 1, 0},
        {4, 4, 3, 1, 1, 0, 1, 0},
        {4, 4, 2, 2, 1, 0, 0, 0},
        {2, 3, 2, 1, 1, 0, 0, 0}
    },
    ...
};
```
Nice, now we have all the tiles properly stored and indexable by 6 bits.

### How can the levels be created?
The least referenceable memory unit is 8 bits, and our indexes are 6 bits, so we still have 2 bits to store some more info. For more variation and also to reuse existing tiles these 2 bits can be used to specify the vertical and horizontal flipping.
```cpp
struct tile_index_t {
    uint8_t index : 6;
    uint8_t flip  : 2;
};

const Level::tile_index_t Level::level[levelH][levelW] PROGMEM =  {
    { {11, 0}, {11, 0}, ... },
    ...
};
```
Defining all those tils by hand would be fairly error prone and tidieus.\
For that I wrote a [python script](https://github.com/AlexandruScutaru/EscapeTheSewer/blob/master/resources/tiles_generator.py) to generate the tiles as c-array from the .bmp tileset. This is helpful if in the future another tileset is to be used.

Same goes for defning a level. 
Best option would be to procedurally create teh levels, didn't manage to do that yet, but it is there in the to do list.
For now they are created using [Tiled](https://www.mapeditor.org/).\
<img src="https://github.com/AlexandruScutaru/EscapeTheSewer/blob/master/readme_resources/tiled.png" alt="tiled.png" width="100%"/>

I created another [python script](https://github.com/AlexandruScutaru/EscapeTheSewer/blob/master/resources/level_generator.py) to take care of the level based on the tiled export.

## Audio
Everything in the game happens sequentially, as no multi-threading can be leveraged on an arduino (there is though something called protothreading, but that is another subject).\
So getting audio, which is very time sensitive, in between game updates is not really ideal.

Here come interrupts.\
[This file](https://github.com/AlexandruScutaru/EscapeTheSewer/blob/master/src/audio.cpp) shows how a tiemr interrupt can be used to drive the sound at specific time intervals, so the buzzer tone is consistent.\
The melodies themselves are being defined in the same manner the tiles are.

# Getting it running
## On Arduino
I created the project using [PlatformIO](https://platformio.org/) for VS Code.\
It is straight forward to get it on the board. Compile and linking is done directly by PlatformIO, but the `upload_port` property from [platformio.ini](https://github.com/AlexandruScutaru/EscapeTheSewer/blob/master/platformio.ini) must be updated accordingly so it finds the board to upload the code to.

## On PC
To render the game I used [SFML](https://www.sfml-dev.org/).\
Get the corresponding SFML for your the platform/compiler. I structure the files in this manner:
```
pc_version
└───deps
    └───include
    │   └───SDML
    │       └───include files
    └───lib
    │   └───lib files
    └───runtime
        └───dll files
```
I have msvc and I wrote a simple [batch script](https://github.com/AlexandruScutaru/EscapeTheSewer/blob/master/pc_version/build.bat) to get the job done quickly.


### Why the PC version
The so called _PC version_ is just the same game logic-wise (it just uses other display/input _strategies_.) that is loaded by `pc_version/main.cpp`.\
I opted to load those _strategies_ statically using `#if defined` macro operator, eg in [main_game.h](https://github.com/AlexandruScutaru/EscapeTheSewer/blob/master/include/main_game.h):
```cpp
#if defined (ARDUINO) || defined (__AVR_ATmega328P__)
    #include "input_manager.h"
    #include "graphics.h"
#else
    #include "../pc_version/input_manager_pc.h"
    #include "../pc_version/graphics_pc.h"
#endif
```
It was done to my work easier.\
I _simulated_ to some extent the 1.8 in LCD display. and due to the fact the logic was common for both this was particularly helpful in being able to debug properly while developing.

I did this by creating a matrix of pixels `std::vector<std::vector<uint16_t>> screen;` (initially I was trying to do something with `std::rotate` over `std::vector`s for the scrolling part, but ended up doing something similar manually. TODO: change it to a `uint16_t screen[][]`).\
Then every action coming from the `common` logic will be applied on a per pixel basis to the screen matrix, which will end up diplayed by SFML.

# Hardware
The handheld itself is a revision of a previous project of mine.\
I created a 1:1 printable pcb design, you can find a pdf of it [here](https://github.com/AlexandruScutaru/EscapeTheSewer/blob/master/readme_resources/pcb_design.pdf).

The best I could do for now for a schematic is this:\
<img src="https://github.com/AlexandruScutaru/EscapeTheSewer/blob/master/readme_resources/schematic.png" alt="schematic.png" width="100%"/>

You can imagine the schematic as being the components laid down on a _see-through etched board_ seen from the top side.
* 6 resistors (10k ohms) used for pulling down the buttons
* 1.8 in TFT LCD screen (11-pin version: VCC, GNG, GND, NC, NC, NC, CLK, SDA, RS, RST, CS, like [this one](https://www.banggood.com/1_8-Inch-TFT-LCD-Display-Module-Color-Screen-SPI-Serial-Port-128+160-p-1566669.html?cur_warehouse=CN))
* the sound comes through an active buzzer. I also connected a resistor (forgot what value, perhaps 220 ohm) to limit the volume a bit
* microSD card reader but it is unused for this project.\
I don't even know if it works together with the LCD due to the way the particular ST7735 driver handles high speed SPI communication)
* the blue lines are simple jumpers on the top side

The unit is powered by a Li-ion battery. The charger is a `TP4056` module and 5v voltage is supplied by a mini dc step up [module](https://www.banggood.com/5pcs-3V-or-3_7V-To-5V-1A-Lithium-Battery-Step-Up-Module-Board-Mini-Mobile-Power-Boost-Charger-Module-With-Undervoltage-Indication-p-1428332.html?cur_warehouse=CN).
As the schematic shows, there's a trace that goes into Arduino pin `A6`. That is coming directly from the battery, before the voltage booster module in order to read the current battery level.

# Credits
What made this project doable on an Arduino Nano is the fast ST7735 lcd driver written by [sumotoy](https://github.com/sumotoy/TFT_ST7735/tree/1.0p1).

Tileset used is based on [F24 Tileset](https://0x72.itch.io/8x8-f24-tileset) made by [0x72](https://0x72.itch.io/).
