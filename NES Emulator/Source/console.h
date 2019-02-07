//
//  console.h
//  NES_Emulator_Go_Port
//
//  Created by Samuel Hunt on 19/02/2018.
//
//

#ifndef NES_Emulator_Go_Port_console_h
#define NES_Emulator_Go_Port_console_h

#include "../JuceLibraryCode/JuceHeader.h"
#include "cpu.h"
#include "ppu.h"
#include "controller.h"
#include "cartridge.h"
#include "Mapper.h"

class Routine
{
public:
    Routine ()
    {
        ram = new Byte[2048];
        for (int i = 0; i < 2048; i++) {
            ram[i] = 0;
        }
    }
    
    Byte * ram;

};

class Console : public Routine {
public:
    
    Console (File rom) : cpu(this), ppu(this)
    {
        palletInit();
        
        
        /*
         
         cartridge, err := LoadNESFile(path)
         if err != nil {
         return nil, err
         }
         ram := make([]byte, 2048)
         controller1 := NewController()
         controller2 := NewController()
         console := Console{
         nil, nil, nil, cartridge, controller1, controller2, nil, ram}
         mapper, err := NewMapper(&console)
         if err != nil {
         return nil, err
         }
         console.Mapper = mapper
         console.CPU = NewCPU(&console)
         console.APU = NewAPU(&console)
         console.PPU = NewPPU(&console)
         return &console, nil
         
         */
    }
    ~Console()
    {
        delete [] ram;
    }
    
    void reset ()
    {
        cpu.Reset();
    }
    int step ()
    {
        int cpuCycles = cpu.step();
        int ppuCycles = cpuCycles * 3;
        
        for (int i = 0; i < ppuCycles; i++) {
  //          mapper->step();
//            console.Mapper.Step();
            ppu.Step();
        }
        for (int i = 0; i < cpuCycles; i++) {
            ///jassertfalse;
//            console.APU.Step()
        }
        return cpuCycles;
    }
    
    int stepFrame ()
    {
        int cpuCycles = 0;
        int64 frame = ppu.Frame;
        if (frame == ppu.Frame) {
            cpuCycles += step();
        }
        return cpuCycles;

    }
    
    void StepSeconds(Float64 seconds ) {
        int cycles = (int) CPUFrequency * seconds;
        while (cycles >= 0) {
            cycles -= step();
        }
    }
    
    
    Image buffer () {
        Image img(Image::PixelFormat::ARGB, pixelWidth, pixelHeight, false);
        for (int x = 0; x < pixelWidth; x++) {
            for (int y = 0; y < pixelHeight; y++) {
                NesColour nc = ppu.front[x][y];
                img.setPixelAt(x, y, Colour(nc.r, nc.g, nc.b, nc.a));
            }
        }
        
        return img;
        
    }
    NesColour backgroundColor () {
        return Palette[ppu.readPalette(0)%64];
    }
    void SetButtons1(bool buttons[8]) {
        controller1.setButtons(buttons);
    }
    void SetButtons2(bool buttons[8]) {
        controller2.setButtons(buttons);
    }
    void SetAudioChannel (Float32 chan) {
//        console.APU.channel = channel
 //apu
    }
    
#warning WILL PICK UP THE AUDIO AND STATE STUFF LATER ON!
    /*
    func (console *Console) SetAudioSampleRate(sampleRate float64) {
        if sampleRate != 0 {
            // Convert samples per second to cpu steps per sample
            console.APU.sampleRate = CPUFrequency / sampleRate
            // Initialize filters
            console.APU.filterChain = FilterChain{
                HighPassFilter(float32(sampleRate), 90),
                HighPassFilter(float32(sampleRate), 440),
                LowPassFilter(float32(sampleRate), 14000),
            }
        } else {
            console.APU.filterChain = nil
        }
    }
     */
    
    
    /*
     
     
     func (console *Console) SaveState(filename string) error {
     dir, _ := path.Split(filename)
     if err := os.MkdirAll(dir, 0755); err != nil {
     return err
     }
     file, err := os.Create(filename)
     if err != nil {
     return err
     }
     defer file.Close()
     encoder := gob.NewEncoder(file)
     return console.Save(encoder)
     }
     
     func (console *Console) Save(encoder *gob.Encoder) error {
     encoder.Encode(console.RAM)
     console.CPU.Save(encoder)
     console.APU.Save(encoder)
     console.PPU.Save(encoder)
     console.Cartridge.Save(encoder)
     console.Mapper.Save(encoder)
     return encoder.Encode(true)
     }
     
     func (console *Console) LoadState(filename string) error {
     file, err := os.Open(filename)
     if err != nil {
     return err
     }
     defer file.Close()
     decoder := gob.NewDecoder(file)
     return console.Load(decoder)
     }
     
     func (console *Console) Load(decoder *gob.Decoder) error {
     decoder.Decode(&console.RAM)
     console.CPU.Load(decoder)
     console.APU.Load(decoder)
     console.PPU.Load(decoder)
     console.Cartridge.Load(decoder)
     console.Mapper.Load(decoder)
     var dummy bool
     if err := decoder.Decode(&dummy); err != nil {
     return err
     }
     return nil
     }
     
     */
    
    
    CPU cpu;
    PPU ppu;
    
    
    
    Controller controller1;
    Controller controller2;
    Cartridge  cartridge;
    

	//APU         *APU

	Mapper *      mapper;
     
    

};

#if 0



    

    

    




#endif



#endif
