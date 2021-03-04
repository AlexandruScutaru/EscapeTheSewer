#include "input_manager.h"

#include <Arduino.h>


void InputManager::processInput() {
    mPrevMask = mMask;
    mMask = 0;
    //arduino nano's A0-A5 pins can be used also as digital pins
    if(digitalRead(A0))
        mMask |= Button::LEFT;
    if(digitalRead(A1))
        mMask |= Button::DOWN;
    if(digitalRead(A2))
        mMask |= Button::UP;
    if(digitalRead(A3))
        mMask |= Button::RIGHT;
    if(digitalRead(A4))
        mMask |= Button::A;
    if(digitalRead(A5))
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
