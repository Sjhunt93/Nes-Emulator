//
//  AudioProcessor.hpp
//  NES Emulator - App
//
//  Created by Samuel Hunt on 05/03/2019.
//

#ifndef AudioProcessor_hpp
#define AudioProcessor_hpp

#include "../JuceLibraryCode/JuceHeader.h"
#include "console.h"

class NESAudioProcessor : public AudioSource
{
public:
    NESAudioProcessor (Console * console);
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate);
    void releaseResources();
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill);
private:
    Console * console;
    float buffer[5012];
    double sRate;
};

#endif /* AudioProcessor_hpp */
