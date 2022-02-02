#include "game.h"
#include "game_runner.h"
#include "level.h"
#include "level_utils.h"
#include "event.h"

#if defined (ARDUINO) || defined (__AVR_ATmega328P__)
    #include "audio.h"

    #define LOOP_CONDITION (mState != LevelState::PLAYER_DIED && mState != LevelState::FINISHED)

    #define BEGIN_DRAW
    #define END_DRAW
    #define POLL_EVENTS

    #define LOG(msg)
#else
    #include "../pc_version/pc_version/logging.h"
    #include "../pc_version/pc_version/audio_pc.h"

    #define LOOP_CONDITION (mGraphics.getWindow()->isOpen() && mState == LevelState::IN_PROGRESS)
 
    #define millis() Graphics::getElapsedTime()
    #define delay(ms) mGraphics.sleep(ms)

    #define BEGIN_DRAW  mGraphics.clear();
    #define END_DRAW mGraphics.display();
    #define POLL_EVENTS mGraphics.pollEvents();
#endif


Game::Game(Level& level)
    : mLevel(level)
    , mGraphics(mLevel)
{
    init();
    mGraphics.registerEvent(Event<StatusBar>(&mStatusBar));
}

Game::~Game() {}


void Game::run() {
    loop();
}

void Game::init() {
    Serial.println("init game");
    Audio::Init();

    mPlayer.setPos(mLevel.startCoords);

    BEGIN_DRAW
    mGraphics.fillScreen();
    LevelUtils::drawEntireLevel(mLevel, mGraphics);
    mStatusBar.draw(mGraphics, true);
    END_DRAW
}

//the game loop seems a bit meh, works for now
//maybe implement a more proper one, handle multiple "physics steps" before redrawing again
//not sure if a fixed timestep is really needed but could be a nice experiment
void Game::loop() {
    Serial.println("game loop");
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
        mState = LevelState::PLAYER_DIED;
    }
}
