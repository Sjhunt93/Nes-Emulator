//
//  Noise.hpp
//  NES Emulator - App
//
//  Created by Samuel Hunt on 21/02/2019.
//

#ifndef Noise_hpp
#define Noise_hpp

#include "Audio.h"




// Noise

class Noise {
public:
    bool enabled;
    bool mode;
    UInt16 shiftRegister;
    bool lengthEnabled;
    Byte lengthValue;
    UInt16 timerPeriod;
    UInt16 timerValue;
    bool envelopeEnabled;
    bool envelopeLoop;
    bool envelopeStart;
    Byte envelopePeriod;
    Byte envelopeValue;
    Byte envelopeVolume;
    Byte constantVolume;
    
    void writeControl(Byte value);
    void writePeriod(Byte value);
    void writeLength(Byte value);
    
    void stepTimer();
    void stepEnvelope();
    void stepLength();
    Byte output();
};



    

    



#endif /* Noise_hpp */
