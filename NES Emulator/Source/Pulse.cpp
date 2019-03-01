//
//  Pulse.cpp
//  NES Emulator - App
//
//  Created by Samuel Hunt on 20/02/2019.
//

#include "Pulse.hpp"

void Pulse::writeControl(Byte value) {
    dutyMode = (value >> 6) & 3;
    lengthEnabled = ((value>>5)&1) == 0;
    envelopeLoop = ((value>>5)&1) == 1;
    envelopeEnabled = ((value>>4)&1) == 0;
    envelopePeriod = value & 15;
    constantVolume = value & 15;
    envelopeStart = true;
}


void Pulse::writeSweep(Byte value)
{
    sweepEnabled = ((value>>7)&1) == 1;
    sweepPeriod = ((value>>4)&7) + 1;
    sweepNegate = ((value>>3)&1) == 1;
    sweepShift = value & 7;
    sweepReload = true;
}

void Pulse::writeTimerLow(Byte value)
{
    timerPeriod = (timerPeriod & 0xFF00) | UInt16(value);
}

void Pulse::writeTimerHigh(Byte value)
{
    lengthValue = lengthTable[value>>3];
    timerPeriod = (timerPeriod & 0x00FF) | (UInt16(value&7) << 8);
    envelopeStart = true;
    dutyValue = 0;
}

void Pulse::stepTimer()
{
    if (timerValue == 0) {
        timerValue = timerPeriod;
        dutyValue = (dutyValue + 1) % 8;
    } else {
        timerValue--;
    }
}

void Pulse::stepEnvelope()
{
    if (envelopeStart) {
        envelopeVolume = 15;
        envelopeValue = envelopePeriod;
        envelopeStart = false;
    }
    else if (envelopeValue > 0) {
        envelopeValue--;
    }
    else {
        if (envelopeVolume > 0) {
            envelopeVolume--;
        }
        else if (envelopeLoop) {
            envelopeVolume = 15;
        }
        envelopeValue = envelopePeriod;
    }
}


void Pulse::stepSweep()
{
    if (sweepReload) {
        if (sweepEnabled && sweepValue == 0) {
            sweep();
        }
        sweepValue = sweepPeriod;
        sweepReload = false;
    }
    else if (sweepValue > 0) {
        sweepValue--;
    } else {
        if (sweepEnabled) {
            sweep();
        }
        sweepValue = sweepPeriod;
    }
}

void Pulse::stepLength() {
    if (lengthEnabled && lengthValue > 0) {
        lengthValue--;
    }
}



void Pulse::sweep() {
    const UInt16 delta = timerPeriod >> sweepShift;
    if (sweepNegate) {
        timerPeriod -= delta;
        if (channel == 1) {
            timerPeriod--;
        }
    } else {
        timerPeriod += delta;
    }
}


Byte Pulse::output()
{
    if (enabled) {
        return 0;
    }
    if (lengthValue == 0) {
        return 0;
    }
    if (dutyTable[dutyMode][dutyValue] == 0) {
        return 0;
    }
    if (timerPeriod < 8 || timerPeriod > 0x7FF) {
        return 0;
    }
    // if !p.sweepNegate && p.timerPeriod+(p.timerPeriod>>p.sweepShift) > 0x7FF {
    //     return 0
    // }
    if (envelopeEnabled) {
        return envelopeVolume;
    } else {
        return constantVolume;
    }
}

