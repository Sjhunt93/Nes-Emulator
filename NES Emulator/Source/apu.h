//
//  apu.h
//  NES_Emulator_Go_Port
//
//  Created by Samuel Hunt on 31/07/2018.
//
//

#ifndef __NES_Emulator_Go_Port__apu__
#define __NES_Emulator_Go_Port__apu__
#include "Audio.h"
//const frameCounterRate = CPUFrequency / 240.0

#include "Filter.hpp"
#include "Triangle.hpp"
#include "Noise.hpp"
#include "Pulse.hpp"
#include "DMC.hpp"


class Console;

class APU {
public:
    
    
    Console *  console;
    float  channel;
    Float64 sampleRate;
    Pulse  pulse1;
    Pulse  pulse2;
    Triangle  triangle;
    Noise  noise;
    DMC  dmc;
    UInt64  cycle;
    Byte  framePeriod;
    Byte  frameValue;
    bool  frameIRQ;
//    FilterChain  filterChain;
    
    float pulseTable[31];
    float tndTable[203];
    
    
    
    
    APU (Console * _console)
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
#warning SORT THIS OUT
//        dmc.cpu = console.CPU;
    }
    
    void Step() {
        const UInt64 cycle1 = cycle;
        cycle++;
        const UInt64 cycle2 = cycle;
//        stepTimer();
        const int f1 = (int) (((Float64)cycle1) / frameCounterRate);
        const int f2 = (int) (((Float64)cycle2) / frameCounterRate);
        if (f1 != f2) {
//            stepFrameCounter();
        }
        const int s1 = (int) (((Float64)cycle1) / sampleRate);
        const int s2 = (int) (((Float64)cycle2) / sampleRate);

        if (s1 != s2) {
            sendSample();
        }
    }
    void sendSample() {
        /*
        const float output = apu.filterChain.Step(apu.output())
        select {
        case apu.channel <- output:
        default:
        }
         */
    }
    float output() {
        float p1 = pulse1.output();
        float p2 = pulse2.output();
        float t = triangle.output();
        float n = noise.output();
        float d = dmc.output();
        float pulseOut = pulseTable[(int)(p1+p2)];
        float tndOut = tndTable[(int)(3*t+2*n+d)];
        return pulseOut + tndOut;
        }

    // mode 0:    mode 1:       function
    // ---------  -----------  -----------------------------
    //  - - - f    - - - - -    IRQ (if bit 6 is clear)
    //  - l - l    l - l - -    Length counter and sweep
    //  e e e e    e e e e -    Envelope and linear counter
    
    void stepFrameCounter() {
        switch (framePeriod) {
        case 4:
            frameValue = (frameValue + 1) % 4;
            switch (frameValue) {
                case 0:
                case 2:
                    stepEnvelope();
                break;
            case 1:
                    stepEnvelope();
                    stepSweep();
                    stepLength();
                    break;
            case 3:
                    stepEnvelope();
                    stepSweep();
                    stepLength();
                    fireIRQ();
                    break;
            }
            break;
        case 5:
            frameValue = (frameValue + 1) % 5;
            switch (frameValue) {
                case 1:
                case 3:
                    stepEnvelope();
                    break;
                case 0:
                case 2:
                    stepEnvelope();
                    stepSweep();
                    stepLength();
                    break;
            }
                break;
        }
    }
    
    

    
    
    void stepTimer() {
        if (cycle%2 == 0) {
            pulse1.stepTimer();
            pulse2.stepTimer();
            noise.stepTimer();
            dmc.stepTimer();
        }
        triangle.stepTimer();
    }
    void stepEnvelope() {
        pulse1.stepEnvelope();
        pulse2.stepEnvelope();
        triangle.stepCounter();
        noise.stepEnvelope();
    }
    void stepSweep() {
        pulse1.stepSweep();
        pulse2.stepSweep();
    }
    
    void stepLength() {
        pulse1.stepLength();
        pulse2.stepLength();
        triangle.stepLength();
        noise.stepLength();
    }
    void fireIRQ() {
        if (frameIRQ) {
            //console.CPU.triggerIRQ()
        }
    }
    Byte readRegister(UInt16 address)  {
        if (address == 0x4015) {
            return readStatus();
        }
        return 0;
    }
    
    
    Byte readStatus() {
        Byte result = 0;
        if (pulse1.lengthValue > 0) {
            result |= 1;
        }
        if (pulse2.lengthValue > 0) {
            result |= 2;
        }
        if (triangle.lengthValue > 0) {
            result |= 4;
        }
        if (noise.lengthValue > 0) {
            result |= 8;
        }
        if (dmc.currentLength > 0) {
            result |= 16;
        }
        return result;
    }
    
    
    void writeControl(Byte value) {
        pulse1.enabled = (value&1) == 1;
        pulse2.enabled = (value&2) == 2;
        triangle.enabled = (value&4) == 4;
        noise.enabled = (value&8) == 8;
        dmc.enabled = (value&16) == 16;
        if (!pulse1.enabled) {
            pulse1.lengthValue = 0;
        }
        if (!pulse2.enabled) {
            pulse2.lengthValue = 0;
        }
        if (!triangle.enabled) {
            triangle.lengthValue = 0;
        }
        if (!noise.enabled) {
            noise.lengthValue = 0;
        }
        if (!dmc.enabled) {
            dmc.currentLength = 0;
        } else {
            if (dmc.currentLength == 0) {
                dmc.restart();
            }
        }
    }
    void writeFrameCounter(Byte value) {
        framePeriod = 4 + (value>>7)&1;
        frameIRQ = ((value>>6)&1) == 0;
        // apu.frameValue = 0
        if (framePeriod == 5) {
            stepEnvelope();
            stepSweep();
            stepLength();
        }
    }

    
    void writeRegister(UInt16 address, Byte value) {
        switch (address) {
        case 0x4000:
                pulse1.writeControl(value);
                break;
        case 0x4001:
                pulse1.writeSweep(value);
                break;
        case 0x4002:
                pulse1.writeTimerLow(value);
                break;
        case 0x4003:
                pulse1.writeTimerHigh(value);
                break;
        case 0x4004:
                pulse2.writeControl(value);
                break;
        case 0x4005:
                pulse2.writeSweep(value);
                break;
        case 0x4006:
                pulse2.writeTimerLow(value);
                break;
        case 0x4007:
                pulse2.writeTimerHigh(value);
                break;
        case 0x4008:
                triangle.writeControl(value);
                break;
        case 0x4009:
        case 0x4010:
                dmc.writeControl(value);
                break;
        case 0x4011:
                dmc.writeValue(value);
                break;
        case 0x4012:
                dmc.writeAddress(value);
                break;
        case 0x4013:
                dmc.writeLength(value);
                break;
        case 0x400A:
                triangle.writeTimerLow(value);
                break;
        case 0x400B:
                triangle.writeTimerHigh(value);
                break;
        case 0x400C:
                noise.writeControl(value);
                break;
        case 0x400D:
        case 0x400E:
                noise.writePeriod(value);
                break;
        case 0x400F:
                noise.writeLength(value);
                break;
        case 0x4015:
                writeControl(value);
                break;
        case 0x4017:
                writeFrameCounter(value);
                break;
            // default:
            //     log.Fatalf("unhandled apu register write at address: 0x%04X", address)
        }
    }

};



#endif /* defined(__NES_Emulator_Go_Port__apu__) */

