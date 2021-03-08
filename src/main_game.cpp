#include "main_game.h"
#include "level.h"

#if defined (ARDUINO) || defined (__AVR_ATmega328P__)
    #define LOOP_CONDITION 1

    #define BEGIN_DRAW
    #define END_DRAW
    #define POLL_EVENTS

    #define LOG(msg)
#else
    #include "../pc_version/logging.h"

    #define LOOP_CONDITION (!mInputManager.isButtonPressed(InputManager::Button::ESC) && mGraphics.getWindow()->isOpen())

    #define millis() Graphics::getElapsedTime()
    #define delay(ms) mGraphics.sleep(ms)

    #define BEGIN_DRAW  mGraphics.clear();
    #define END_DRAW mGraphics.display();
    #define POLL_EVENTS mGraphics.pollEvents();
#endif


MainGame::MainGame() {
    init();
}

MainGame::~MainGame() {}


void MainGame::run() {
    loop();
}

void MainGame::init() {
    Level::init();
    Level::setGraphics(&mGraphics);
    mPlayer.setPos(Level::mStartCoords);

    BEGIN_DRAW
    mGraphics.fillScreen();
    Level::drawEntireLevel();
    END_DRAW
}

//the game loop seems a bit meh, works for now
//maybe implement a more proper one, handle multiple "physics steps" before redrawing again
//not sure if a fixed timestep is really needed but could be a nice experiment
void MainGame::loop() {
    uint32_t targetFrameTicks = 1000 / 30;
    uint32_t prevTicks = 0;
    uint32_t newTicks = 0;
    uint32_t frameTicks = 0;
    int32_t difference = 0;
    float delta = 0.0f;

    prevTicks = millis();
    while(LOOP_CONDITION) {
        newTicks = millis();
        frameTicks = newTicks - prevTicks;
        prevTicks = newTicks;
        delta = frameTicks / (float)targetFrameTicks;

        POLL_EVENTS
        mInputManager.processInput();
        update(delta);
        draw();

        difference = targetFrameTicks - (millis() - newTicks);
        if(difference > 0)
            delay(difference);
    }
}

void MainGame::draw() {
    BEGIN_DRAW
    Level::draw();
    mPlayer.draw(mGraphics);
    END_DRAW
}

void MainGame::update(float dt) {
    Level::update(dt);
    mPlayer.update(mInputManager, dt);
}
