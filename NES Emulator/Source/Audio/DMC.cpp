//
//  DMC.cpp
//  NES Emulator - App
//
//  Created by Samuel Hunt on 21/02/2019.
//

#include "DMC.hpp"
#include "cpu.h"

void DMC::stepReader() {
    if (currentLength > 0 && bitCount == 0) {
        cpu->stall += 4;
        shiftRegister = cpu->memory.read(currentAddress);
        
        bitCount = 8;
        currentAddress++;
        if (currentAddress == 0) {
            currentAddress = 0x8000;
        }
        currentLength--;
        if (currentLength == 0 && loop) {
            restart();
        }
    }
}

