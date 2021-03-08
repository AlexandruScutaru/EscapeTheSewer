#ifndef DATA_H
#define DATA_H

#include "vector.h"
#include "slime.h"
#include "coin.h"

#if defined (ARDUINO) || defined (__AVR_ATmega328P__)
    #include <Arduino.h>
#else
    #define PROGMEM
#endif

#include <stdint.h>

#define TILE_SIZE  8

#define ANIM_WALK_START   32
#define ANIM_IDLE_START   36
#define ANIM_FALL_START   40
#define ANIM_JUMP_START   42
#define ANIM_CLIMB_START  44
#define ANIM_ATTACK_START 46

#define ANIM_WALK_FRAMES   4
#define ANIM_IDLE_FRAMES   4
#define ANIM_FALL_FRAMES   2
#define ANIM_JUMP_FRAMES   2
#define ANIM_CLIMB_FRAMES  2
#define ANIM_ATTACK_FRAMES 2

#define TILE_LADDER                   19
#define TILE_EMPTY                    63
#define TILE_NON_COLLIDABLE_THRESHOLD 19


class Graphics;


class Level {
public:
    enum class EntityType {
        SLIME,
        COIN,
        END,
        NONE,
    };

    union tile_row_t {
        struct row_s{
            uint32_t col1 : 4;
            uint32_t col2 : 4;
            uint32_t col3 : 4;
            uint32_t col4 : 4;
            uint32_t col5 : 4;
            uint32_t col6 : 4;
            uint32_t col7 : 4;
            uint32_t col8 : 4;
        } row;
        uint32_t packed;
    };

    typedef tile_row_t tile_t[8];

    union tile_index_t {
        struct tile_index_s {
            uint8_t index : 6;
            uint8_t flip  : 2;
        } tile_index;
        uint8_t packed;
    };

    static void init();
    static void update(float dt);
    static void cleanPrevDraw();
    static void draw();
    static void drawEntireLevel();
    static void setGraphics(Graphics* graphics);

    static EntityType getCollidedEntity(const vec2& pos, size_t& idx);
    static void removeEntity(EntityType entt, size_t idx);
    static void hitEntity(EntityType entt, size_t idx, float dmg, float force);

    static uint8_t getTileByPosition(uint16_t x, uint16_t y);
    static tile_index_t getTileByIndex(uint8_t i, uint8_t j);
    static tile_row_t getTileRow(uint8_t tileIndex, uint8_t rowIndex);

    const static uint8_t levelW = 20;
    const static uint8_t levelH = 16;
    const static uint16_t colors[16] /*PROGMEM*/;

    static vec2 mStartCoords;

private:
    static void populateSpecialObjects();
    static bool aabb(const vec2& pos1, const vec2& pos2);

    const static tile_index_t level[levelH][levelW] /*PROGMEM*/;
    const static tile_t tiles[] PROGMEM;

    //I haven't tried making these polymorphic, not sure if I want to add vtable overhead on arduino
    //these entities are dealt with dozens of times a second...
    static vector<Slime> mSlimes;
    static vector<Coin> mCoins;

    static vec2 mEndCoords;

    static Graphics* mGraphics; 

};

#endif // DATA_H