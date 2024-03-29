#include "game.h"
#include "level.h"
#include "level_utils.h"
#include "event.h"
#include "vec2.inl"


#if defined (ARDUINO) || defined (__AVR_ATmega328P__)
    #include "audio.h"
    #include "level_loader.h"
    #include <avr/wdt.h>

    #define LOOP_CONDITION (mState != LevelState::FINISHED)

    #define BEGIN_DRAW
    #define END_DRAW
    #define POLL_EVENTS
#else
    #include "../pc_version/pc_version/audio_pc.h"
    #include "../pc_version/pc_version/level_loader_pc.h"

    #define LOOP_CONDITION (mGraphics.getWindow()->isOpen() && mState == LevelState::IN_PROGRESS)
 
    #define millis() Graphics::getElapsedTime()
    #define delay(ms) mGraphics.sleep(ms)

    #define BEGIN_DRAW  mGraphics.clear();
    #define END_DRAW mGraphics.display();
    #define POLL_EVENTS mGraphics.pollEvents();
#endif


Game::Game()
    : mGraphics(mLevel)
{
    mGraphics.registerEvent(Event<StatusBar>(&mStatusBar, &StatusBar::onRedraw));
}

Game::~Game() {}

void Game::init() {
    mState = LevelState::IN_PROGRESS;
    mGraphics.reset();
    Audio::Init();

    mPlayer.setPos(mLevel.startCoords);

    BEGIN_DRAW
    mGraphics.fillScreen();
    LevelUtils::drawEntireLevel(mLevel, mGraphics);
    mStatusBar.draw(mGraphics, true);
    END_DRAW
}

void Game::run() {
    while (LevelLoader::LoadLevel(mCurrentLevel, mLevel)) {
        init();
        loop();
        mCurrentLevel++;
    }


#if defined (ARDUINO) || defined (__AVR_ATmega328P__)
    // the "screensaver" made a reappearence
    Audio::Disable();
    uint16_t color = 0x38A6;
    for(;;) {
        mGraphics.fillScreen(color);
        mStatusBar.update(0.0f);
        mStatusBar.draw(mGraphics, true);
        color += 0x045A;
        delay(1000);
    }
#endif
}

//the game loop seems a bit meh, works for now
//maybe implement a more proper one, handle multiple "physics steps" before redrawing again
//not sure if a fixed timestep is really needed but could be a nice experiment
void Game::loop() {
    uint32_t targetFrameTicks = 1000 / 24;
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
        handleCollision();
        draw();

        difference = targetFrameTicks - (millis() - newTicks);
        if(difference > 0)
            delay(difference);
    }
}

void Game::draw() {
    BEGIN_DRAW
    
    //clean prev draws
    for (uint8_t i = 0; i < mLevel.enemies.size(); i++) {
      mLevel.enemies[i].cleanPrevDraw(mLevel, mGraphics);
    }

    mPlayer.cleanPrevDraw(mLevel, mGraphics);

    //draw special objects
    for (uint8_t i = 0; i < mLevel.pickups.size(); i++) {
      mLevel.pickups[i].draw(mGraphics);
    }
    for (uint8_t i = 0; i < mLevel.enemies.size(); i++) {
      mLevel.enemies[i].draw(mGraphics);
    }

    mPlayer.draw(mGraphics);
    mStatusBar.draw(mGraphics);
    END_DRAW
}

void Game::update(float dt) {
    for (uint8_t i = 0; i < mLevel.enemies.size(); i++) {
      mLevel.enemies[i].update(mLevel, mGraphics, dt);
    }

    mPlayer.update(mInputManager, mLevel, mGraphics, dt);

    mStatusBar.update(dt);
    mStatusBar.setPlayerHp(mPlayer.getHp());
}

void Game::handleCollision() {
    LevelUtils::collideWithPickups(mLevel, mGraphics, mPlayer);

    if (LevelUtils::collideWithEnd(mLevel, mPlayer.getPos())) {
        mState = LevelState::FINISHED;
        return;
    }

    if (LevelUtils::collideWithEnemies(mLevel, mPlayer)) {
        // if player died, sadly no nmore space to add message to the screen, just restart :)
        wdt_enable(WDTO_15MS);
        for(;;);
    }
}
