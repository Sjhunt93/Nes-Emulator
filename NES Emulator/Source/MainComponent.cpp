/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"

//==============================================================================
#include "Mapper.h"


MainComponent::MainComponent() : Thread("nes"), NES(f), audioProcessor(&NES)
{
    
    setSize (1600, 800);
    
    Cartridge cart = loadNESFile(File("/Users/sj4-hunt/Documents/roms/3538 NES ROMs/Europe/Super Mario Bros (E).nes"));
    //   Cartridge cart = loadNESFile(File("/Users/sj4-hunt/Downloads/3538 NES ROMs/Europe/Super Mario Bros (E).nes"));
    //  Cartridge cart = loadNESFile(File("/Users/sj4-hunt/Downloads/3538 NES ROMs/USA/Joypad Test Cartridge (U).nes"));
    
    //
    //Cartridge cart = loadNESFile(File("/Users/sj4-hunt/Downloads/3538 NES ROMs/USA/Ice Climber (U).nes"));
    NES.mapper = Mapper::create(cart);
    NES.cpu.init();
    NES.cpu.Reset();
    NES.cartridge = cart;
    
    NES.ppu.memory.makePallette();

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
    addAndMakeVisible(xScroll);
    speedSlider.setRange(1.0, 400.0);
    speedSlider.setValue(100);
 
    xScroll.setRange(-32, 32, 1);
    
    startTimer(1000);
    setAudioChannels(2, 2);
    AudioDeviceManager::AudioDeviceSetup aws = deviceManager.getAudioDeviceSetup();
    aws.bufferSize = 64;
    deviceManager.setAudioDeviceSetup(aws, false);


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
    
    //going to draw the pattern table
    
    
    g.setColour(Colours::red);
    g.drawLine(512, 0, 512, getHeight());
//    std::cout << "repainted \n";
    
#if 0
    {
    float xPos = 530;
    float yPos = 10;
    
    Image img(Image::PixelFormat::RGB, 8, 8, false);
    static const int cMap[4] = {0, 40, 70, 255};
    for (int i = 0; i < NES.ppu.memory.patterns.size(); i++) {
        PatternTable pTable = NES.ppu.memory.patterns[i];
        
        for (int x = 0; x < 8; x++) {
            for (int y = 0; y < 8; y++) {
                img.setPixelAt(x, y, Colour::greyLevel(cMap[pTable.data[x][y]] / 255.0));
                g.drawImage(img, xPos, yPos, 16, 16, 0, 0, 8, 8);
                
            }
        }
        xPos += 18;
        if (xPos+18 >= getWidth()) {
            xPos = 530;
            yPos += 18;
        }
        
    }
    }
#endif
    
    {
        float yPos = 10;
        const int size = 16;

        Image img(Image::PixelFormat::RGB, 8, 8, false);
//        static const int cMap[4] = {0, 40, 70, 255};
        static const int cMap[4] = {0, 40, 70, 255};
        g.setColour(Colours::black);
        g.setFont(8);
        int counter = 0;
        for (int row = 0; row < 30; row++) {
            float xPos = 530;

            for (int col = 0; col < 64; col++) {
//                const int nTableAddress = (0x2000+ row + (col+32));
                

//                const int nTableAddress = (0x2000    + col + (row*32));
                int nTableAddress = ((col < 32 ? 0x2000 : 0x2400) + (col%32) + (row*32));
                
#if 0
                int nTableAddress = 0;
                if (col < 32) {
                    int nTableAddress = 0x2000 + col + (row*32);
                }
                else {
                    int nTableAddress = 0x2400 + (col-32) + (row*32);
                }
#endif
                
                counter++;

                const int index = NES.ppu.memory.read(nTableAddress);
                
                const int actualAddress = 0x1000 + index * 16;
                const int mappedAddress = 256 + index;
//                PatternTable pTable = NES.ppu.memory.patterns[mappedAddress+xScroll.getValue()];
                PatternTable pTable = NES.ppu.memory.tableForAddressBase(actualAddress);
                
//                const int attributreAddress = ((col < 32 ? 0x23C0 : 0x27C0) + ((col%32) + (row*32))/4);
                Byte attributeByte = NES.ppu.memory.attributeByteForXY(col,row);
                
                /* copy pixel values into imgs*/
                
                for (int x = 0; x < 8; x++) {
                    for (int y = 0; y < 8; y++) {
                        int c = (pTable.data[x][y] | (attributeByte << 2));// | 0x10;
//                        const bool b = (c%4) != 0;
                        if ((c%4) == 0) {
                            c = 0;
                        }

                        NesColour color =  Palette[NES.ppu.readPalette(c)];
//                        img.setPixelAt(x, y, Colour::greyLevel(cMap[pTable.data[x][y]] / 255.0));
                        img.setPixelAt(x, y, Colour(color.r, color.g, color.b));
                    }
                }
                g.drawImage(img, xPos, yPos, size , size , 0, 0, 8, 8);
                xPos += size;
            }
            yPos += size;
        }
        const int xCoarse = NES.ppu.vramAddress.xCoarseScroll;
        const int xFine = NES.ppu.xFineScroll;
        const int pos = 530 + (xCoarse * size) + (xFine * 2);
        g.setColour(Colours::orange.withAlpha(0.7f));
        
//        g.drawLine(pos, 0, pos, 200, 4.0);
    }


    
    {
        int x = 530;
        int y = 530;
        for (int i = 0; i < 32; i++) {
            NesColour c = Palette[NES.ppu.readPalette(i)];
            g.setColour(Colour(c.r, c.g, c.b));
            g.fillRect(x, y, 16, 16);
            x += 17;
            if (i == 15) {
                x = 530;
                y += 17;
            }
        }
        

    }
    
    
}

void MainComponent::resized()
{
    // This is called when the MainComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
    
    screen.setBounds(0, 0, pixelWidth * 2, pixelHeight*2);
    speedSlider.setBounds(0, screen.getHeight() + 10, 300, 30);
    xScroll.setBounds(0, speedSlider.getY() + 40, speedSlider.getWidth(), speedSlider.getHeight());
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
    if (KeyPress::isKeyCurrentlyDown(67)) {
        repaint();
        
    }
    
    
    return true;
}

void MainComponent::timerCallback()
{
   repaint();
}

void MainComponent::sliderValueChanged (Slider* slider)
{
    
}

//#define FOWARD(ROOT, CLASS, func  )

void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    audioProcessor.prepareToPlay(samplesPerBlockExpected, sampleRate);
}
void MainComponent::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
    audioProcessor.getNextAudioBlock(bufferToFill);
}
void MainComponent::releaseResources()
{
    audioProcessor.releaseResources();
}
