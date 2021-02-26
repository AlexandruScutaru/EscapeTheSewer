#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H
                        
#include <stdint.h>


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

    void processInput();
    bool isButtonPressed(Button button);
    bool isButtonHeldDown(Button button);
    bool isButtonReleased(Button button);
    uint8_t getMask();

private:
    uint8_t mMask = 0;
    uint8_t mPrevMask = 0;
 
};

#endif
