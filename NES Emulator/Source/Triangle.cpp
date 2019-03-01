//
//  Triangle.cpp
//  NES Emulator - App
//
//  Created by Samuel Hunt on 21/02/2019.
//

#include "Triangle.hpp"


void Triangle::writeControl(Byte value)
{
    lengthEnabled = ((value>>7)&1) == 0;
    counterPeriod = value & 0x7F;
}

void Triangle::writeTimerLow(Byte value)
{
    timerPeriod = (timerPeriod & 0xFF00) | UInt16(value);
}

void Triangle::writeTimerHigh(Byte value)
{
    lengthValue = lengthTable[value>>3];
    timerPeriod = (timerPeriod & 0x00FF) | (UInt16(value&7) << 8);
    timerValue = timerPeriod;
    counterReload = true;
}


void Triangle::stepTimer() {
    if (timerValue == 0) {
        timerValue = timerPeriod;
        if (lengthValue > 0 && counterValue > 0) {
            dutyValue = (dutyValue + 1) % 32;
        }
    } else {
        timerValue--;
    }
}

void Triangle::stepLength() {
    if (lengthEnabled && lengthValue > 0) {
        lengthValue--;
    }
}

void Triangle::stepCounter()
{
    if (counterReload) {
        counterValue = counterPeriod;
    } else if (counterValue > 0) {
        counterValue--;
    }
    if (lengthEnabled) {
        counterReload = false;
    }
}

Byte Triangle::output() {
    if (!enabled) {
        return 0;
    }
    if (lengthValue == 0) {
        return 0;
    }
    if (counterValue == 0) {
        return 0;
    }
    return triangleTable[dutyValue];
}

