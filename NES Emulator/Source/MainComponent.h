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

class MainComponent   : public Component, public Thread, public KeyListener, public Timer
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent();
    
    void paint (Graphics&) override;
    void resized() override;
    
    
    void run();
    
    bool keyPressed (const KeyPress& key,
                     Component* originatingComponent);
    
    bool keyStateChanged (bool isKeyDown, Component* originatingComponent);
    
    void timerCallback();

    
    File f;
    Console NES;
    bool buttons[8];
    Slider speedSlider;
private:
    ScreenComponent screen;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
