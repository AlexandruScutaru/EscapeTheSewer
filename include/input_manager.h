#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H
                        
#include <Arduino.h>

class InputManager{
public:
    enum Button {
        LEFT  =  0x01,
        RIGHT =  0x02,
        DOWN  =  0x04,
        UP    =  0x08,
        A     =  0x10,
        B     =  0x20
    };

    void processInput() {
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

    bool isButtonPressed(Button button) {
        return (bool)(mMask & button);
    }

    bool isButtonHeldDown(Button button) {
        return (mPrevMask & button) && (mMask & button);
    }

    bool isButtonReleased(Button button) {
        return (mPrevMask & button) && !(mMask & button);
    }

    uint8_t getMask() {
        return mMask;
    }

private:
    uint8_t mMask = 0;
    uint8_t mPrevMask = 0;
 
};

#endif
