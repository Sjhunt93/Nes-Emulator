//
//  Mapper.cpp
//  NES_Emulator_Go_Port
//
//  Created by Samuel Hunt on 02/07/2018.
//
//

#include "Mapper.h"

/*static*/ Mapper * Mapper::create (Cartridge cart)
{
    switch (cart.Mapper) {
        case 0:
            return new Mapper2(cart);
            break;
            
        default:
            break;
    }
    jassertfalse;
    return nullptr;
}


// mapper 2

Mapper2::Mapper2 (Cartridge _cart) : cart(_cart)
{
    prgBanks = cart.NumPRG;// / 0x4000;
	prgBank1 = 0;
	prgBank2 = prgBanks - 1;
}

void Mapper2::write (UInt16 address, Byte value)
{

    if (address < 0x2000) {
        cart.CHR[address] = value;
    }
    else if (address >= 0x8000) {
        prgBank1 = int(value) % prgBanks;
    }
    else if (address >= 0x6000) {
		const int index = int(address) - 0x6000;
		cart.SRAM[index] = value;
    }
    else {
        jassertfalse;
    }
}
Byte Mapper2::read (UInt16 address)
{
    if (address < 0x2000) {
        jassert(address < cart.chrAlloc);
        return cart.CHR[address];
    }
    else if (address >= 0xC000)
    {
        const int index = prgBank2*0x4000 + int(address-0xC000);
        jassert(index < cart.prgAlloc);
		return cart.PRG[index];
    }
    else if (address >= 0x8000)
    {
		const int index = prgBank1*0x4000 + int(address-0x8000);
                jassert(index < cart.prgAlloc);
		return cart.PRG[index];
    }
    else if (address >= 0x6000)
    {
		const int index = int(address) - 0x6000;
		return cart.SRAM[index];
    }
    else {
        jassertfalse;
    }
    return 0;
}