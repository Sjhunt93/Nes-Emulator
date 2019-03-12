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
#include <map>





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

union BMask8 {
    struct {
        Byte b0 : 1;
        Byte b1 : 1;
        Byte b2 : 1;
        Byte b3 : 1;
        Byte b4 : 1;
        Byte b5 : 1;
        Byte b6 : 1;
        Byte b7 : 1;
    };
    Byte data;
    
};

struct PatternTable {
    Byte data[8][8];
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
    Byte attributeByteForXY (const Byte x, const Byte y);
    
    
    static bool checkBit (Byte input, Byte bit)
    {
        Byte flag = 1 << bit;
        return (input & flag);
    }
    void makePallette () {
        patterns.clear();
        for (int index = 0; index < 0x2000; ) {
            
            PatternTable pTable;

            for (int y = 0; y < 8; y++) {
                Byte rowA = read(index);
                Byte rowB = read(index+8);
                for (int x = 0; x < 8; x++) {
                    int a = checkBit(rowA, x);
                    int b = checkBit(rowB, x);
                    pTable.data[7-x][y] = a+(b<<1);
                }
                index++;
            }
            patterns.push_back(pTable);
        }
    }
    
    PatternTable tableForAddressBase (UInt16 address)
    {
        
        auto iterator = tiles.find(address);
        if (iterator != tiles.end()) {
            return tiles.at(address);
        }
        else {
        
        PatternTable pTable;
        
        int index = address;
        for (int y = 0; y < 8; y++) {
            Byte rowA = read(index);
            Byte rowB = read(index+8);
            for (int x = 0; x < 8; x++) {
                int a = checkBit(rowA, x);
                int b = checkBit(rowB, x);
                pTable.data[7-x][y] = a+(b<<1);
            }
            index++;
        }
            tiles.insert(std::make_pair(address, pTable)); //cashing up each img.
        return pTable;
        }
    }
    
    std::vector<PatternTable> patterns;

    
private:
    Console * console;
    std::map<UInt16, PatternTable> tiles;
};
        
#endif
