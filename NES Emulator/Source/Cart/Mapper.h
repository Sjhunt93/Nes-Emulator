//
//  Mapper.h
//  NES_Emulator_Go_Port
//
//  Created by Samuel Hunt on 02/07/2018.
//
//

#ifndef __NES_Emulator_Go_Port__Mapper__
#define __NES_Emulator_Go_Port__Mapper__

#include "../JuceLibraryCode/JuceHeader.h"
#include "cartridge.h"


class Mapper {
public:
    virtual ~Mapper () {}
    virtual void write (UInt16 address, Byte value) {}
    virtual Byte read (UInt16 address) {return 0;}
    
    static Mapper * create (Cartridge cart);
};


class Mapper2 : public Mapper {
public:
    Mapper2 (Cartridge cart);
    void write (UInt16 address, Byte value);
    Byte read (UInt16 address);
    
    int prgBanks;
	int prgBank1;
	int prgBank2;
private:
    Cartridge cart;
};









#endif /* defined(__NES_Emulator_Go_Port__Mapper__) */
