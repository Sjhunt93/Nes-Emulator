//
//  Memory.cpp
//  NES_Emulator_Go_Port
//
//  Created by Samuel Hunt on 19/02/2018.
//
//

#include "Memory.h"
#include "console.h"


static UInt16 MirrorAddress (Byte mode, UInt16 address)
{
    jassert(mode == 1);
    jassert(address >= 0x2000);
    address = (address - 0x2000) % 0x1000;
    UInt16 table = address / 0x400;
    UInt16 offset = address % 0x400;
    jassert(table < 4);
    return 0x2000 + (MirrorLookup[mode][table]*0x400) + offset;

}

Byte cpuMemory::read (UInt16 address) {
    
    //I have left the originals in for now incase this thing fucks about!
    if (address < 0x2000) {
        return console->ram[address%0x0800];
    }
    else if (address < 0x4000) {
        return console->ppu.readRegister(0x2000 + (address%8));
    }
    else if (address == 0x4014) {
        return console->ppu.readRegister(address);
    }
    else if (address == 0x4015) {
        
        //            return mem.console.APU.readRegister(address)
    }
    else if (address == 0x4016) {
        return console->controller1.read();
    }
    else if (address == 0x4017) {
        return console->controller2.read();
    }
    
    else if (address < 0x6000) {
        // TODO: I/O registers
    }
    else if (address >= 0x6000) {
        return console->mapper->read(address);
    }
    else {
        printf("unhandled cpu memory read at address: 0x%04X", address);
    }
    return 0;
}



void cpuMemory::write (UInt16 address, Byte data)
{

    
    if (address < 0x2000) {
        console->ram[address%0x0800] = data;
    }
    else if (address < 0x4000) {
        console->ppu.writeRegister(0x2000+(address%8), data);
    }
    else if (address < 0x4014) {
        //        mem.console.APU.writeRegister(address, value)

    }
    else if (address == 0x4014) {
        console->ppu.writeRegister(address, data);

    }
    
    else if (address == 0x4015) {
        //        mem.console.APU.writeRegister(address, value)

    }
    else if (address == 0x4016) {
        console->controller1.write(data);
        console->controller2.write(data);
    }
    else if (address == 0x4017) {
//        mem.console.APU.writeRegister(address, value)
    }
    
    else if (address < 0x6000) {
        // TODO: I/O registers
    }
    else if (address >= 0x6000) {
        console->mapper->write(address, data);
    }
    else {
        printf("unhandled cpu memory read at address: 0x%04X", address);
    }

    
    
}



Byte ppuMemory::read (UInt16 address)
{
    
     
    address = address % 0x4000;
    
    if (address < 0x2000) {
//        return console->cpu.memory.read(address);
        return console->mapper->read(address);
    }
    else if (address < 0x3F00) {
        int mode = console->cartridge.Mirror;
        int addressReturned = MirrorAddress(mode, address);
        return console->ppu.nameTableData[addressReturned % 2048];
    }
    else if (address < 0x4000) {
        
        return console->ppu.readPalette(address % 32);
    }
    else {
        printf("unhandled ppu memory read at address: 0x%04X", address);
    }
    
    return 0;
}
void ppuMemory::write (UInt16 address, Byte data)
{
    address = address % 0x4000;

    if (address < 0x2000) {
        return console->mapper->write(address, data);
    }
    else if (address < 0x3F00) {
        const int mode = console->cartridge.Mirror;
        int addressReturned = MirrorAddress(mode, address);
        int writeAdress = addressReturned%2048;
        jassert(writeAdress < 2048);
        console->ppu.nameTableData[writeAdress] = data;
    }
    else if (address < 0x4000) {
        console->ppu.writePalette(address%32, data);
    }
    else {
        printf("unhandled ppu memory read at address: 0x%04X", address);
    }
    
    
    
}


Byte ppuMemory::attributeByteForXY (const Byte x, const Byte y)
{
    /*
     pretend that the
     */
    
    //64 * 60
    //16 * 15
//    const uint16 address = (x/4) + (y*16);
    Byte toReturn = 0;
    if (x < 32) {
        if (y < 30) {
            toReturn = read(0x23C0+(x/4) + (y*16));
        }
        else {
            toReturn = read(0x2BC0+(x/4) + ((y-30)*16));
        }
    }
    else {
        if (y < 30) {
            toReturn = read(0x27C0+((x-32)/4) + (y*16));
        }
        else {
            toReturn = read(0x2FC0+((x-32)/4) + ((y-30)*16));
        }
    }
    int xP = (x % 4) / 2;
    int yP = (y % 4) / 2;
    if (yP == 0) {
        return ((xP == 0) ? toReturn : toReturn >> 2) & 0x3;
    }
    else {
        return ((xP == 0) ? toReturn >> 4 : toReturn >> 6) & 0x3;
    }
}
