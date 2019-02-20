//
//  Pulse.hpp
//  NES Emulator - App
//
//  Created by Samuel Hunt on 20/02/2019.
//

#ifndef Pulse_hpp
#define Pulse_hpp

#include "../JuceLibraryCode/JuceHeader.h"

// Pulse

class Pulse {
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
};


#if 0


    
    func (p *Pulse) output() byte {
        if !p.enabled {
            return 0
        }
        if p.lengthValue == 0 {
            return 0
        }
        if dutyTable[p.dutyMode][p.dutyValue] == 0 {
            return 0
        }
        if p.timerPeriod < 8 || p.timerPeriod > 0x7FF {
            return 0
        }
        // if !p.sweepNegate && p.timerPeriod+(p.timerPeriod>>p.sweepShift) > 0x7FF {
        //     return 0
        // }
        if p.envelopeEnabled {
            return p.envelopeVolume
        } else {
            return p.constantVolume
        }
    }
#endif
#endif /* Pulse_hpp */
