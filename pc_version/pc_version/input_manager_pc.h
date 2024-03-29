#ifndef INPUT_MANAGER_PC_H
#define INPUT_MANAGER_PC_H

#include <stdint.h>
#include <memory>


class InputManager{
public:
    enum Button {
        LEFT  =  0x01,
        RIGHT =  0x02,
        DOWN  =  0x04,
        UP    =  0x08,
        A     =  0x10,
        B     =  0x20,
    };

    void processInput();
    bool isButtonPressed(Button button);
    bool isButtonHeldDown(Button button);
    bool isButtonReleased(Button button);
    bool wasButtonPressedNow(Button button);

    uint8_t getMask();

private:
    uint8_t mMask = 0;
    uint8_t mPrevMask = 0;
 
};

#endif
