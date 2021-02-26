#include "main_game.h"

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
    Serial.begin(9600);

    mGraphics.fillScreen();
    mGraphics.drawLevel();
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
    while(1) {
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
    mGraphics.drawFillRect(oldPosX, oldPosY, SIZE, SIZE, COLOR_BROWN_DARKER);
    mGraphics.drawTile(animFrame>>2, posX, posY, SIZE, flipSprite);
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
        if(mGraphics.getTileAt(posX + 0.0f, oldPosY + 0.0f) != 29 || mGraphics.getTileAt(posX + 0.0f, oldPosY + SIZE-1 /*0.9f*/) != 29) {
            posX = (((uint16_t)posX >> 3) + 1) << 3;
            velX = 0.0f;
        }
    } else {
        if(mGraphics.getTileAt(posX + SIZE /*1.0f*/, oldPosY + 0.0f) != 29 || mGraphics.getTileAt(posX + SIZE /*1.0f*/, oldPosY + SIZE-1 /*0.9f*/) != 29) {
            posX = ((uint16_t)posX >> 3) << 3;
            velX = 0.0f;
        }
    }

    onGround = false;
    if(velY <= 0) {
        if(mGraphics.getTileAt(posX + 0.0f, posY) != 29 || mGraphics.getTileAt(posX + SIZE-1 /*0.9f*/, posY) != 29) {
            posY = (((uint16_t)posY >> 3) + 1) << 3;
            velY = 0.0f;
        }
    } else {
        if(mGraphics.getTileAt(posX + 0.0f, posY + SIZE /*1.0f*/) != 29 || mGraphics.getTileAt(posX + SIZE-1 /*0.9f*/, posY + SIZE/*1.0f*/) != 29) {
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
