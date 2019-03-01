//
//  Triangle.hpp
//  NES Emulator - App
//
//  Created by Samuel Hunt on 21/02/2019.
//

#ifndef Triangle_hpp
#define Triangle_hpp

#include "Audio.h"


class Triangle {
public:
    bool enabled;
    bool lengthEnabled;
    Byte lengthValue;
    UInt16 timerPeriod;
    UInt16 timerValue;
    Byte dutyValue;
    Byte counterPeriod;
    Byte counterValue;
    bool counterReload;
    
    
    void writeControl(Byte value);
    void writeTimerLow(Byte value);
    void writeTimerHigh(Byte value);
    
    void stepTimer();
    void stepLength();
    void stepCounter();
    Byte output();
};



    
    
    



#endif /* Triangle_hpp */
