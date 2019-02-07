//
//  memory.h
//  NES_Emulator_Go_Port
//
//  Created by Samuel Hunt on 19/02/2018.
//
//

#ifndef NES_Emulator_Go_Port_memory_h
#define NES_Emulator_Go_Port_memory_h

#include "../JuceLibraryCode/JuceHeader.h"





// Mirroring Modes

enum eMirrors {
       MirrorHorizontal = 0,
       MirrorVertical   = 1,
       MirrorSingle0    = 2,
       MirrorSingle1    = 3,
       MirrorFour       = 4,
};

static const UInt16 MirrorLookup [5][4]{
    {0, 0, 1, 1},
    {0, 1, 0, 1},
    {0, 0, 0, 0},
    {1, 1, 1, 1},
    {0, 1, 2, 3},
};



/*
 
 type Memory interface {
 Read(address uint16) byte
 Write(address uint16, value byte)
 }

 */
//This class is preety pointless and could be handled by the console or cpu...
class Console;


class cpuMemory {
public:
    cpuMemory (Console * _console)
    {
        console = _console;
    }
    
    
    Byte read (UInt16 address);
    void write (UInt16 address, Byte data);
    
    
    
private:
    Console * console;
};


        // PPU Memory Map
class ppuMemory {
public:
    ppuMemory (Console * _console)
    {
        console = _console;
    }
    
    
    Byte read (UInt16 address);
    void write (UInt16 address, Byte data);
    
    
    
private:
    Console * console;
};
        
#endif
