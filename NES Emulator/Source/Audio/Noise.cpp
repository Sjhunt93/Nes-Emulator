//
//  Noise.cpp
//  NES Emulator - App
//
//  Created by Samuel Hunt on 21/02/2019.
//

#include "Noise.hpp"

void Noise::writeControl(Byte value) {
    lengthEnabled = ((value>>5)&1) == 0;
    envelopeLoop = ((value>>5)&1) == 1;
    envelopeEnabled = ((value>>4)&1) == 0;
    envelopePeriod = (value & 15);
    constantVolume = (value & 15);
    envelopeStart = true;
}


void Noise::writePeriod(Byte value) {
    mode = (value&0x80) == 0x80;
    timerPeriod = noiseTable[value&0x0F];
}

void Noise::writeLength(Byte value)
{
    lengthValue = lengthTable[value>>3];
    envelopeStart = true;
}


void Noise::stepTimer() {
    if (timerValue == 0) {
        timerValue = timerPeriod;
        Byte shift;
        if (mode) {
            shift = 6;
        } else {
            shift = 1;
        }
        Byte b1 = shiftRegister & 1;
        Byte b2 = (shiftRegister >> shift) & 1;
        shiftRegister >>= 1;
        shiftRegister |= (b1 ^ b2) << 14;
    }
    else {
        timerValue--;
    }
}


void Noise::stepEnvelope() {
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
        } else if (envelopeLoop) {
            envelopeVolume = 15;
        }
        envelopeValue = envelopePeriod;
    }
}

void Noise::stepLength() {
    if (lengthEnabled && lengthValue > 0) {
        lengthValue--;
    }
}

Byte Noise::output() {
    if (!enabled) {
        return 0;
    }
    if (lengthValue == 0) {
        return 0;
    }
    if ((shiftRegister&1) == 1) {
        return 0;
    }
    if (envelopeEnabled) {
        return envelopeVolume;
    }
    else {
        return constantVolume;
    }
}


