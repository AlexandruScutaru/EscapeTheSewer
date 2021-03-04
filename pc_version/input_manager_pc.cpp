#include "input_manager_pc.h"

#include <SFML/Window.hpp>


void InputManager::processInput() {
    mPrevMask = mMask;
    mMask = 0;

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        mMask |= Button::LEFT;
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
        mMask |= Button::DOWN;
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
        mMask |= Button::UP;
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        mMask |= Button::RIGHT;
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
        mMask |= Button::A;
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::X))
        mMask |= Button::B;
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
        mMask |= Button::ESC;
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
