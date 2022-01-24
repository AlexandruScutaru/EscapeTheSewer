#ifndef LEVEL_H
#define LEVEL_H

#include "vector.h"
//#include "vec2.h"
#include "enemy.h"
#include "pickup.h"

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
#define TILE_EMPTY                    48
#define TILE_NON_COLLIDABLE_THRESHOLD 19

#define LEVEL_MAX_W 50
#define LEVEL_MAX_H 15

class Graphics;
class Player;

class Level {
public:
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

            uint16_t operator[] (size_t idx) const {
                switch (idx) {
                    case 1: return col1;
                    case 2: return col2;
                    case 3: return col3;
                    case 4: return col4;
                    case 5: return col5;
                    case 6: return col6;
                    case 7: return col7;
                    case 8: return col8;
                    default:
                        //maybe an assert or something
                        return 0;
                }
            }
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

    static void clear();
    static bool loadNextLevel();
    static bool writeCurrentLevelIndex();
    static void update(float dt);
    static void cleanPrevDrawSpecialObjects();
    static void drawSpecialObjects();
    static void drawEntireLevel();
    static void setGraphics(Graphics* graphics);

    static bool collideWithEnd(const vec2& pos);
    static bool collideWithLevel(vec2& pos, const vec2& oldPos, vec2& velocity, const vec2& velocityToSet, bool* flip = nullptr, bool* onGround = nullptr, uint16_t* ladderXpos = nullptr);
    static void collideWithPickups(Player& player);
    static bool collideWithEnemies(Player& player);
    static void cleanPrevDraw(const vec2& oldPos);

    static void removeEnemy(size_t idx);
    static void removePickup(size_t idx);
    static bool hitEnemy(const vec2& pos, float dmg, float force);

    static uint8_t getTileByPosition(uint16_t x, uint16_t y);
    static tile_index_t getTileByIndex(uint8_t i, uint8_t j);
    static tile_row_t getTileRow(uint8_t tileIndex, uint8_t rowIndex);

    static uint8_t levelW;
    static uint8_t levelH;
    const static uint16_t colors[16] /*PROGMEM*/;
    static vec2 mStartCoords;

private:
    friend class LevelLoader;

    static bool aabb(const vec2& pos1, const vec2& pos2);

    static tile_index_t level[LEVEL_MAX_H][LEVEL_MAX_W];
    const static tile_t tiles[] PROGMEM;

    //I haven't tried making these polymorphic, not sure if I want to add vtable overhead on arduino
    static vector<Enemy> mEnemies;
    static vector<Pickup> mPickups;

    static vec2 mEndCoords;
    static Graphics* mGraphics;

    static uint8_t mCurrentLevel;

};

#endif // LEVEL_H