//
//  apu.cpp
//  NES_Emulator_Go_Port
//
//  Created by Samuel Hunt on 31/07/2018.
//
//

#include "apu.h"
#include "console.h"
APU::APU (Console * _console)
{
    for (int i = 0; i < 31; i++) {
        pulseTable[i] = 95.52 / (8128.0/(float)(i) + 100);
    }
    for (int i = 0; i < 203; i++) {
        tndTable[i] = 163.67 / (24329.0/(float)(i) + 100);
    }
    
    console = _console;
    noise.shiftRegister = 1;
    pulse1.channel = 1;
    pulse2.channel = 2;
    dmc.cpu = &console->cpu;
    
    timeL = Time::currentTimeMillis();
}


void APU::fireIRQ() {
    if (frameIRQ) {
        console->cpu.triggerIRQ();
    }
    }
