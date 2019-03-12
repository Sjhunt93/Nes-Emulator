/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "console.h"
#include "ScreenComponent.h"
#include "ines.h"
#include "AudioProcessor.hpp"
//==============================================================================
/*
 This component lives inside our window, and this is where you should put all
 your controls and content.
 */


class NameTableView : public Component
{
    
    void paint (Graphics & g)
    {
        
    }
};

class MainComponent : public AudioAppComponent, public Thread, public KeyListener, public Timer, public Slider::Listener
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent();
    
    void paint (Graphics&) override;
    void resized() override;
    
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;
    
    
    void run() override;
    
    bool keyPressed (const KeyPress& key,
                     Component* originatingComponent) override;
    
    bool keyStateChanged (bool isKeyDown, Component* originatingComponent) override;
    
    void timerCallback() override;

    void sliderValueChanged (Slider* slider) override;

    
    
private:
    ScreenComponent screen;
    
    File f;
    Console NES;


    NESAudioProcessor audioProcessor;
    bool buttons[8];
    Slider speedSlider;
    Slider xScroll;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
