//
//  AudioProcessor.cpp
//  NES Emulator - App
//
//  Created by Samuel Hunt on 05/03/2019.
//

#include "AudioProcessor.hpp"

NESAudioProcessor::NESAudioProcessor (Console * _console)
{
    console = _console;
    //    setAudioChannels (2, 2);
    for (int i = 0; i < 5012; i++) {
        buffer[i] = 0;
    }
    
    
}

void NESAudioProcessor::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    //    console->apu.sampleRate = sampleRate;
    console->setAudioSampleRate(sampleRate);
    sRate = sampleRate;
}
void NESAudioProcessor::releaseResources()
{
    
}
void NESAudioProcessor::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
    //output()
    
    
    console->apu.sampleStep(sRate, bufferToFill.numSamples, buffer);
    
    for (int i = 0; i < bufferToFill.numSamples; ++i)
    {
        const float sample = buffer[i];// console->apu.output();
        for (int j = bufferToFill.buffer->getNumChannels(); --j >= 0;) {
            *bufferToFill.buffer->getWritePointer (j, bufferToFill.startSample + i) = sample;
            
        }
        
        
    }
}
