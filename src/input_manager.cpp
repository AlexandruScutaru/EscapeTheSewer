#include "input_manager.h"

#include <Arduino.h>


InputManager::InputManager() {
    pinMode( 2, INPUT_PULLUP);
    pinMode( 3, INPUT_PULLUP);
    pinMode( 4, INPUT_PULLUP);
    pinMode( 5, INPUT_PULLUP);
    pinMode(A2, INPUT_PULLUP);
    pinMode( 7, INPUT_PULLUP);
}

void InputManager::processInput() {
    mPrevMask = mMask;
    mMask = 0;
    //arduino nano's A0-A5 pins can be used also as digital pins
    if(!digitalRead(2))
        mMask |= Button::LEFT;
    if(!digitalRead(3))
        mMask |= Button::DOWN;
    if(!digitalRead(4))
        mMask |= Button::UP;
    if(!digitalRead(A2))
        mMask |= Button::RIGHT;
    if(!digitalRead(5))
        mMask |= Button::A;
    if(!digitalRead(7))
        mMask |= Button::B;
}

bool InputManager::isButtonPressed(Button button) {
    return (bool)(mMask & button);
}

bool InputManager::isButtonHeldDown(Button button) {
    return (mPrevMask & button) && (mMask & button);
}

bool InputManager::isButtonReleased(Button button) {
    return (mPrevMask & button) && !(mMask & button);
}

bool InputManager::wasButtonPressedNow(Button button) {
    return (mMask & button) && !(mPrevMask & button);
}

uint8_t InputManager::getMask() {
    return mMask;
}
