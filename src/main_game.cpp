#include "main_game.h"
#include "data.h"

#if defined (ARDUINO) || defined (__AVR_ATmega328P__)
    #define LOOP_CONDITION 1

    #define BEGIN_DRAW
    #define END_DRAW

    #define LOG(msg) Serial.println(msg)

#else
    #include "../pc_version/logging.h"

    #define LOOP_CONDITION !mInputManager.isButtonPressed(InputManager::Button::ESC)

    #define min(a,b) ((a)<(b)?(a):(b))
    #define max(a,b) ((a)>(b)?(a):(b))
    #define millis() mGraphics.getElapsedTime();
    #define delay(ms) mGraphics.sleep(ms);

    #define BEGIN_DRAW  mGraphics.getWindow()->clear();mGraphics.drawLevel();
    #define END_DRAW mGraphics.getWindow()->display();
#endif

#define SIZE                8
#define WALK_SPEED       1.5f
#define GRAVITY          1.2f
#define JUMP_FORCE       8.0f


MainGame::MainGame() {
    init();
}

MainGame::~MainGame() {}


void MainGame::run() {
    loop();
}

void MainGame::init() {
    BEGIN_DRAW
    mGraphics.fillScreen();
    mGraphics.drawLevel();
    END_DRAW
}

//the game loop seems a bit meh, works for now
//maybe implement a more proper one, handle multiple "physics steps" before redrawing again
//not sure if a fixed timestep is really needed but could be a nice experiment
void MainGame::loop() {
    int targetFrameTicks = 1000 / 30;
    uint32_t prevTicks = 0;
    uint32_t newTicks = 0;
    uint32_t frameTicks = 0;
    int32_t difference;
    float delta;

    prevTicks = millis();
    while(LOOP_CONDITION) {
        newTicks = millis();
        frameTicks = newTicks - prevTicks;
        prevTicks = newTicks;
        delta = frameTicks / (float)targetFrameTicks;

        mInputManager.processInput();
        update(delta);
        draw();

        difference = targetFrameTicks - frameTicks;
        if(difference > 0)
            delay(difference);
    }
}

void MainGame::draw() {
    BEGIN_DRAW
    mGraphics.drawFillRect(oldPosX, oldPosY, SIZE, SIZE, COLOR_BROWN_DARKER);
    mGraphics.drawTile(animFrame>>2, posX, posY, SIZE, flipSprite);
    END_DRAW
}

void MainGame::update(float dt) {
    oldPosX = posX;
    oldPosY = posY;

    if(mInputManager.isButtonPressed(InputManager::Button::A) && onGround){
        velY = -JUMP_FORCE;
    }

    if (mInputManager.isButtonPressed(InputManager::Button::LEFT)) {
        velX = -WALK_SPEED;
        flipSprite = true;
    } else if (mInputManager.isButtonPressed(InputManager::Button::RIGHT)) {
        velX = WALK_SPEED;
        flipSprite = false;
    } else {
        velX = 0.0f;
    }

    velY = min(2*GRAVITY, velY + GRAVITY * dt);
    
    posX = min(max(0, posX + velX * dt), 152);
    posY += velY * dt;
    if(posY <= 0.0f) {
        velY = 0.0f;
        posY = 0.0f;
    }
    posY = min(posY, 120);

    if(velX <= 0) {
        if(Data::getTileByPosition(posX + 0.0f, oldPosY + 0.0f) != 29 || Data::getTileByPosition(posX + 0.0f, oldPosY + SIZE-1 /*0.9f*/) != 29) {
            posX = (((uint16_t)posX >> 3) + 1) << 3;
            velX = 0.0f;
        }
    } else {
        if(Data::getTileByPosition(posX + SIZE /*1.0f*/, oldPosY + 0.0f) != 29 || Data::getTileByPosition(posX + SIZE /*1.0f*/, oldPosY + SIZE-1 /*0.9f*/) != 29) {
            posX = ((uint16_t)posX >> 3) << 3;
            velX = 0.0f;
        }
    }

    onGround = false;
    if(velY <= 0) {
        if(Data::getTileByPosition(posX + 0.0f, posY) != 29 || Data::getTileByPosition(posX + SIZE-1 /*0.9f*/, posY) != 29) {
            posY = (((uint16_t)posY >> 3) + 1) << 3;
            velY = 0.0f;
        }
    } else {
        if(Data::getTileByPosition(posX + 0.0f, posY + SIZE /*1.0f*/) != 29 || Data::getTileByPosition(posX + SIZE-1 /*0.9f*/, posY + SIZE/*1.0f*/) != 29) {
            posY = ((uint16_t)posY >> 3) << 3;
            velY = 0.0f;
            onGround = true;
            if(posY >= 120) {
                velY = 0.0f;
                posY = 120;
                onGround = true;
            }
        }
    }

    //implement proper animation handling
    //  decoupled from framerate
    //  time based, anim frame rate configurable
    //  change animation based on eventual player FSM
    animFrame++;
    if(animFrame >> 2 >= 4)
        animFrame = 0;
}
