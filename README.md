# EscapeTheSewer
## Simple side-scrolling platformer game for an Arduino handheld console.

This [youtube video](https://www.youtube.com/watch?v=chjQD4TIpNc) shows a rather poor recording of it running on the Arduino.

Below is a sample of it running on pc.\
<img src="https://github.com/AlexandruScutaru/EscapeTheSewer/blob/master/readme_resources/EscapeTheSewer.gif" alt="EscapeTheSewer.gif" width="60%"/>

# Software
## Graphics
### Managing the tiles

The first thing that would seem fine to do is to save these tiles on a micro SD card and just read the tiles whenever needed and display them.\
That works fine but it is so slow that you can see the pixels coming in, that is not an experience you can build a game on.

Next logical thing would be to store the tiles on Arduino's memory and read them directly from there.\
There's a slight issue, Arduino Nano has a flash of 32KB and the tileset I ended up using is around 12KB. That would waste almost half the space. The rest needs to store the bootloader, the LCD driver, all the code that drives the game, the melodies and some level data...

I then thought about defining a limited color palette. I'm not that good of an artist so I searched a bit through [itch.io](https://itch.io/) and I stumbled across [this tileset](https://0x72.itch.io/8x8-f24-tileset) which is based on a 24 colors pallete.\
`24` wasn't going to cut for my needs to decrease memory footprint so I resorted to the next smaller power of 2, and that is `16` colors.\
I extracted 16 colors from the palette and redraw the tileset with those.

The color palette has 16 colors, this means there are required 4 bits to index a color.
The tiles used in this project are all 8x8. So there are 8x8x4 bits required to define a tile. This adds up to a total of 32 bytes per tile.
Following this, a bitfield struct of 4 bytes in size can be declared to store a tile row. 8 of these are needed to define a tile.

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
Using indices from the palette, a tile can now be defined like this:
```cpp
tile_t tile = {
    { 2, 3, 2, 2, 1, 0, 0, 0 },
    { 4, 4, 3, 1, 1, 1, 0, 2 },
    { 4, 3, 2, 3, 1, 1, 0, 0 },
    { 2, 3, 2, 2, 1, 0, 0, 0 },
    { 4, 3, 2, 2, 1, 0, 1, 0 },
    { 4, 4, 3, 1, 1, 0, 1, 0 },
    { 4, 4, 2, 2, 1, 0, 0, 0 },
    { 2, 3, 2, 1, 1, 0, 0, 0 }
};
```
The tile are stored as an array in arduino's `PROGMEM`.
The tileset is limited to 64 tiles, this adds up to 2KB to store the entire tileset.

But defining up to 64 tiles by inputing every pixel/color index is very tedious and error prone.\
For that I wrote a [python script](https://github.com/AlexandruScutaru/EscapeTheSewer/blob/master/resources/tiles_generator.py). This takes an input tileset, eg <img src=https://github.com/AlexandruScutaru/EscapeTheSewer/blob/master/resources/f24.bmp alt="f24.bmp" width="30%"/>  and generates the tiles as a c-array.

### Managing the levels
Now we have an array of max 64 tiles, which means a tile is indexable by 6 bits.\
The least addressable memory unit is 8 bits, and our indexes are 6 bits, so we still have 2 bits to store some more info. For more variation and also to reuse existing tiles in different situations, these 2 bits can be used to specify the vertical and horizontal flipping.\
A bitfield struct is used and a level ends up as a matrix of `tile_index_t`.
```cpp
struct tile_index_t {
    uint8_t index : 6;
    uint8_t flip  : 2;
};

Level::tile_index_t Level::level[15][50] =  {
    { {11, 0}, {11, 0}, ... },
    ...
};
```
Same as the tiles array, it is quite tedious to define the levels by hand.\
Best option would be to procedurally create the levels, but I am already reaching the flash limit of the board.\
So they are manually created using [Tiled](https://www.mapeditor.org/).

<img src="https://github.com/AlexandruScutaru/EscapeTheSewer/blob/master/readme_resources/tiled.png" alt="tiled.png" width="100%"/>

I other scripts [level_generator_byte_array.py](https://github.com/AlexandruScutaru/EscapeTheSewer/blob/master/resources/level_generator_byte_array.py) and [level_generator_binary_file.py](https://github.com/AlexandruScutaru/EscapeTheSewer/blob/master/resources/level_generator_binary_file.py) to take care of the levels based on the tiled exports.

The binary file is used by the pc_version.\
To visualize it the format I wrote this file [levels_kaitai_format.ksy](https://github.com/AlexandruScutaru/EscapeTheSewer/blob/master/resources/levels_kaitai_format.ksy). Navigate to [ide.kaitai.io](https://ide.kaitai.io/) and drag and drop the `levels.bin` and `levels_kaitai_format.ksy` from [resources](https://github.com/AlexandruScutaru/EscapeTheSewer/tree/master/resources) directory in the ide's `Local Storage` to explore it.

For Arduino I generate a header file containing the same contents as a byte array. With this litle [helper](https://github.com/AlexandruScutaru/EscapeTheSewer/blob/master/include/level_writer.h) I save it to an [Atmel AT24C128 EEPROM](https://ww1.microchip.com/downloads/en/DeviceDoc/doc0670.pdf). It has a storage of 128k bits (16KB), plenty enough for several levels.\
Everytime arduino starts it loads the first level, when end is reached it tries to read the next one until all of them have been read.

## Audio
Everything in the game happens sequentially, as no multi-threading can be leveraged on an arduino (there is though something called protothreading, but that is another subject).\
So getting audio, which is very time sensitive, in between some game updates that may happen at different times is not really ideal.

Here come interrupts.\
[This file](https://github.com/AlexandruScutaru/EscapeTheSewer/blob/master/src/audio.cpp) shows how a timer interrupt can be used to drive the sound at specific time intervals, so the _background melody_ is consistent.\
The melodies themselves are being defined in the same manner the tiles are.

# Getting it running
## On Arduino
I created the project using [PlatformIO](https://platformio.org/) for VS Code.\
It is straight forward to get it on the board. Clone the repo and init and update the ST7735 display library submodule. Compile, linking and uploading are handled directly by PlatformIO, given the `upload_port` property from [platformio.ini](https://github.com/AlexandruScutaru/EscapeTheSewer/blob/master/platformio.ini) is set accordingly so it finds the board to upload the code to.

## On PC
To render the game I used [SFML](https://www.sfml-dev.org/). And I created a small Visual Studio project.\
TODO: change to a plain cmake project and use it to generate VS solution if needed etc.

### Why the PC version
The so called _PC version_ is just the same game logic-wise (it just uses other display/input _strategies_.).\
I opted to load those _strategies_ statically using `#if defined` macro operator, eg in [game.h](https://github.com/AlexandruScutaru/EscapeTheSewer/blob/master/include/game.h):
```cpp
#if defined (ARDUINO) || defined (__AVR_ATmega328P__)
    #include "input_manager.h"
    #include "graphics.h"
#else
    #include "../pc_version/input_manager_pc.h"
    #include "../pc_version/graphics_pc.h"
#endif
```
It was done to make the development easier without the need to always upload the code and use Serial instrumentation to get an idea of what's going wrong.\
I also tried to _simulate_ the LCD display to some extent to be closer to the real thing. Every action coming from the _common_ logic will be applied on a per pixel basis to the screen matrix, which will end up diplayed by SFML.

# Hardware
WIP

# Credits
What made this project doable on an Arduino Nano is the fast ST7735 lcd driver written by [sumotoy](https://github.com/sumotoy/TFT_ST7735/tree/1.0p1).

Tileset used is based on [F24 Tileset](https://0x72.itch.io/8x8-f24-tileset) made by [0x72](https://0x72.itch.io/).
