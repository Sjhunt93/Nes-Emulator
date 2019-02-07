/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"

//==============================================================================
#include "Mapper.h"


MainComponent::MainComponent() : Thread("nes"), NES(f) {
    setSize (600, 600);
    
    Cartridge cart = loadNESFile(File("/Users/sj4-hunt/Documents/roms/3538 NES ROMs/Europe/Super Mario Bros (E).nes"));
    //   Cartridge cart = loadNESFile(File("/Users/sj4-hunt/Downloads/3538 NES ROMs/Europe/Super Mario Bros (E).nes"));
    //  Cartridge cart = loadNESFile(File("/Users/sj4-hunt/Downloads/3538 NES ROMs/USA/Joypad Test Cartridge (U).nes"));
    
    //
    //Cartridge cart = loadNESFile(File("/Users/sj4-hunt/Downloads/3538 NES ROMs/USA/Ice Climber (U).nes"));
    NES.mapper = Mapper::create(cart);
    NES.cpu.init();
    NES.cpu.Reset();
    NES.cartridge = cart;
    startThread(2);
    
    addKeyListener(this);
    setWantsKeyboardFocus(true);
    
    for (int i = 0; i < 8; i++) {
        buttons[i] = 0;
        
    }
    
    
    
    addAndMakeVisible(screen);
    screen.setPPU(&NES.ppu);
    //    AffineTransform aft = screen.getTransform();
    //    aft = aft.rotated(3.14);
    //    screen.setTransform(aft);
    addAndMakeVisible(speedSlider);
    speedSlider.setRange(1.0, 400.0);
    speedSlider.setValue(100);
}

MainComponent::~MainComponent()
{
}

void MainComponent::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
    
    g.setFont (Font (16.0f));
    g.setColour (Colours::white);
    
    //   g.drawLine(0, getHeight() - 10, <#float endX#>, <#float endY#>)
    
    //NES.ppu.nameTableData
    
    //    g.drawImage(img, 0, 0, img.getWidth() * 2, img.getHeight() * 2, 0, 0, img.getWidth(), img.getHeight());
    
}

void MainComponent::resized()
{
    // This is called when the MainComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
    
    screen.setBounds(0, 0, pixelWidth * 2, pixelHeight*2);
    speedSlider.setBounds(0, screen.getHeight() + 10, 100, 30);
}

//void MainComponent::update()
//{
//    //img = NES.buffer();
//    //repaint();
//}
//
//void MainComponent::timerCallback()
//{
////    repaint();
//   // NES.StepSeconds(1.0/60.0);
//    img = NES.buffer();
//    repaint();
//
//   // NES.cpu.printInstruction();
//
//}

void MainComponent::run()
{
    int current = Time::getMillisecondCounter();
    const int difference = 14;
    while (!threadShouldExit()) {
        usleep(1000);
        if (Time::getMillisecondCounter() > current+difference) {
            const double speed = speedSlider.getValue() / 100.0;
            NES.StepSeconds(0.020 * speed);
            current = Time::getMillisecondCounter();
            
        }
        
    }
}


bool MainComponent::keyPressed (const KeyPress& key,
                                       Component* originatingComponent)
{
    if (key.getKeyCode() == KeyPress::leftKey) {
        NES.controller1.buttons[ButtonLeft] = 1;
        buttons[ButtonLeft] = 1;
    }
    if (key.getKeyCode() == KeyPress::rightKey) {
        NES.controller1.buttons[ButtonRight] = 1;
        buttons[ButtonRight] = 1;
    }
    if (key.getKeyCode() == KeyPress::upKey) {
        NES.controller1.buttons[ButtonUp] = 1;
        buttons[ButtonUp] = 1;
    }
    if (key.getKeyCode() == KeyPress::downKey) {
        NES.controller1.buttons[ButtonDown] = 1;
        buttons[ButtonDown] = 1;
    }
    if (key.getKeyCode() == 65) {
        NES.controller1.buttons[ButtonB] = 1;
        buttons[ButtonB] = 1;
    }
    if (key.getKeyCode() == KeyPress::spaceKey) {
        NES.controller1.buttons[ButtonA] = 1;
        buttons[ButtonA] = 1;
    }
    if (key.getKeyCode() == KeyPress::returnKey) {
        NES.controller1.buttons[ButtonStart] = 1;
        buttons[ButtonStart] = 1;
        //   NES.ppu.print();
    }
    
    if (key.getKeyCode() == 49) { //1
        NES.ppu.disableTileMap = !NES.ppu.disableTileMap;
    }
    if (key.getKeyCode() == 50) { //2
        NES.ppu.disableSprites = !NES.ppu.disableSprites;
    }
    //printf("%i \n", key.getKeyCode());
    return true;
}

bool MainComponent::keyStateChanged (bool isKeyDown, Component* originatingComponent)
{
    if (buttons[ButtonStart] && !KeyPress::isKeyCurrentlyDown(KeyPress::returnKey) && !isKeyDown) {
        NES.controller1.buttons[ButtonStart] = 0;
        buttons[ButtonStart] = 0;
    }
    if (buttons[ButtonLeft] && !KeyPress::isKeyCurrentlyDown(KeyPress::leftKey) && !isKeyDown) {
        NES.controller1.buttons[ButtonLeft] = 0;
        buttons[ButtonLeft] = 0;
    }
    if (buttons[ButtonRight] && !KeyPress::isKeyCurrentlyDown(KeyPress::rightKey) && !isKeyDown) {
        NES.controller1.buttons[ButtonRight] = 0;
        buttons[ButtonRight] = 0;
        
    }
    if (buttons[ButtonUp] && !KeyPress::isKeyCurrentlyDown(KeyPress::upKey) && !isKeyDown) {
        NES.controller1.buttons[ButtonUp] = 0;
        buttons[ButtonUp] = 0;
        
    }
    if (buttons[ButtonDown] && !KeyPress::isKeyCurrentlyDown(KeyPress::downKey) && !isKeyDown) {
        NES.controller1.buttons[ButtonDown] = 0;
        buttons[ButtonDown] = 0;
        
    }
    if (buttons[ButtonA] && !KeyPress::isKeyCurrentlyDown(KeyPress::spaceKey) && !isKeyDown) {
        NES.controller1.buttons[ButtonA] = 0;
        buttons[ButtonA] = 0;
        
    }
    if (buttons[ButtonB] && !KeyPress::isKeyCurrentlyDown(65) && !isKeyDown) {
        NES.controller1.buttons[ButtonB] = 0;
        buttons[ButtonB] = 0;
        
    }
    
    return true;
}
