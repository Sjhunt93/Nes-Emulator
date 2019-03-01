//
//  Pulse.hpp
//  NES Emulator - App
//
//  Created by Samuel Hunt on 20/02/2019.
//

#ifndef Pulse_hpp
#define Pulse_hpp

#include "../JuceLibraryCode/JuceHeader.h"
#include "Audio.h"
// Pulse

class Pulse {
    public:
    bool   enabled;
    Byte   channel;
    bool   lengthEnabled;
    Byte   lengthValue;
    UInt16   timerPeriod;
    UInt16   timerValue;
    Byte   dutyMode;
    Byte   dutyValue;
    bool   sweepReload;
    bool   sweepEnabled;
    bool   sweepNegate;
    Byte   sweepShift;
    Byte   sweepPeriod;
    Byte   sweepValue;
    bool   envelopeEnabled;
    bool   envelopeLoop;
    bool   envelopeStart;
    Byte   envelopePeriod;
    Byte   envelopeValue;
    Byte   envelopeVolume;
    Byte   constantVolume;
    
    void writeControl(Byte value);
    void writeSweep(Byte value);

    void writeTimerLow(Byte value);
    void writeTimerHigh(Byte value);
    
    void stepTimer();
    void stepEnvelope();
    void stepSweep();
    void stepLength();
    void sweep();
    
    Byte output();
};


#endif /* Pulse_hpp */
