//
//  DMC.hpp
//  NES Emulator - App
//
//  Created by Samuel Hunt on 21/02/2019.
//

#ifndef DMC_hpp
#define DMC_hpp

#include "Audio.h"

class CPU;
class DMC {
public:
    CPU *    cpu;
    bool enabled;
    Byte value;
    UInt16 sampleAddress;
    UInt16 sampleLength;
    UInt16 currentAddress;
    UInt16 currentLength;
    Byte shiftRegister;
    Byte bitCount;
    Byte tickPeriod;
    Byte tickValue;
    bool loop;
    bool irq;
    
    
    
    
    void writeControl(Byte _value) {
        irq = (_value&0x80) == 0x80;
        loop = (_value&0x40) == 0x40;
        tickPeriod = dmcTable[_value&0x0F];
    }
    void writeValue(Byte _value) {
        value = _value & 0x7F;
    }
    void writeAddress(Byte _value) {
        // Sample address = %11AAAAAA.AA000000
        sampleAddress = 0xC000 | (UInt16(_value) << 6);
    }
    void writeLength(Byte value ) {
        // Sample length = %0000LLLL.LLLL0001
        sampleLength = (UInt16(value) << 4) | 1;
    }
    void restart() {
        currentAddress = sampleAddress;
        currentLength = sampleLength;
    }
    void stepTimer() {
        if (!enabled) {
            return;
        }
        stepReader();
        if (tickValue == 0) {
            tickValue = tickPeriod;
            stepShifter();
        } else {
            tickValue--;
        }
    }
    
    
    void stepReader();
    
    void stepShifter() {
        if (bitCount == 0) {
            return;
        }
        if ((shiftRegister&1) == 1) {
            if (value <= 125) {
                value += 2;
            }
        }
        else {
            if (value >= 2) {
                value -= 2;
            }
        }
        shiftRegister >>= 1;
        bitCount--;
    }
    
    Byte output()  {
        return value;
    }
    
};
#endif /* DMC_hpp */
