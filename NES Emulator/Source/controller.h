//
//  controller.h
//  NES_Emulator_Go_Port
//
//  Created by Samuel Hunt on 19/02/2018.
//
//

#ifndef __NES_Emulator_Go_Port__controller__
#define __NES_Emulator_Go_Port__controller__


#include "../JuceLibraryCode/JuceHeader.h"


enum Buttons {
       ButtonA = 0,
       ButtonB,
       ButtonSelect,
       ButtonStart,
       ButtonUp,
       ButtonDown,
       ButtonLeft,
       ButtonRight,
};


class Controller
{
public:
    bool buttons[8];
    Byte index;
    Byte strobe;

    
    Controller ()
    {
        index = 0;
        strobe = 0;
        for (int i = 0; i < 8; i++) {
            buttons[i] = 0;
        }
//        buttons[ButtonStart] = 1;
    }
    
    
    void setButtons(bool _buttons[8])
    {
        for (int i = 0; i < 8; i++) {
            buttons[i] = _buttons[i];
        }
    }
    Byte read ()
    {
        Byte value = 0;
        if (index < 8 && buttons[index]) {
            value = 1;
        }
        index++;
        if ((strobe & 1) == 1) {
            index = 0;
        }
        return value;
    }
    void write (Byte value)
    {
        strobe = value;
        if ((strobe&1) == 1) {
            index = 0;
        }
    }
};
#endif /* defined(__NES_Emulator_Go_Port__controller__) */
