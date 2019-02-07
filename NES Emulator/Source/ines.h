//
//  ines.h
//  NES_Emulator_Go_Port
//
//  Created by Samuel Hunt on 21/06/2018.
//
//

#ifndef __NES_Emulator_Go_Port__ines__
#define __NES_Emulator_Go_Port__ines__

#include <iostream>
#include "cartridge.h"

struct iNESFileHeader {
    uint32 Magic; // iNES magic number
	Byte NumPRG;    // number of PRG-ROM banks (16KB each)
	Byte NumCHR;       // number of CHR-ROM banks (8KB each)
	Byte Control1;    // control bits
	Byte Control2;    // control bits
	Byte NumRAM;       // PRG-RAM size (x 8KB)
	Byte Padding[7]; // unused padding

};



const uint32 iNESFileMagic = 0x1a53454e;


// LoadNESFile reads an iNES file (.nes) and returns a Cartridge on success.
// http://wiki.nesdev.com/w/index.php/INES
// http://nesdev.com/NESDoc.pdf (page 28)
static Cartridge loadNESFile (File fPath ) {
    
    if (!fPath.existsAsFile()) {
        jassertfalse;
    }
    
    FileInputStream fStream(fPath);
    jassert(fStream.openedOk());
    iNESFileHeader header;
    fStream.read(&header, sizeof(iNESFileHeader));
    if (header.Magic != iNESFileMagic) {
        jassertfalse;
    }
    
    
	// mapper type
	Byte mapper1 = header.Control1 >> 4;
	Byte mapper2 = header.Control2 >> 4;
	Byte mapper = mapper1 | mapper2<<4;
    
	// mirroring type
	Byte mirror1 = header.Control1 & 1;
	Byte mirror2 = (header.Control1 >> 3) & 1;
	Byte mirror = mirror1 | mirror2<<1;
    
	// battery-backed RAM
	Byte battery = (header.Control1 >> 1) & 1;
    
	// read trainer if present (unused)
    /*
	if header.Control1&4 == 4 {
		trainer := make([]byte, 512)
		if _, err := io.ReadFull(file, trainer); err != nil {
			return nil, err
		}
	}
     */
    Cartridge cart;
    cart.PRG = new Byte[(header.NumPRG)*16384];
	// read prg-rom bank(s)
    printf("Remaining byte %lli %lli \n", fStream.getPosition(), fStream.getTotalLength());

    for (int i = 0; i < (header.NumPRG)*16384; i++) {
        cart.PRG[i] = fStream.readByte();
    }
    
//    const int sizeRead = fStream.read(&cart.PRG, (header.NumPRG)*16384);
//    printf("%i \n", sizeRead);
	// read chr-rom bank(s)

    
    if (header.NumCHR == 0) {
        cart.CHR = new Byte[8192];
    }
    else {
        printf("Remaining byte %lli %lli \n", fStream.getPosition(), fStream.getTotalLength());
        cart.CHR = new Byte[(header.NumCHR)*8192];
        
        //fStream.read(&cart.CHR, (header.NumCHR)*8192);
        for (int i = 0; i < (header.NumCHR)*8192; i++) {
            cart.CHR[i] = fStream.readByte();
        }
    }
    
    cart.NumPRG = (header.NumPRG);// * 16384;
    cart.NumCHR = (header.NumCHR);// * 8192;
    
    cart.Mapper = mapper;
    cart.Mirror = mirror;
    cart.Battery = battery;
    
    jassert(fStream.isExhausted());
    
    return cart;
    
	// success
//	return NewCartridge(prg, chr, mapper, mirror, battery), nil
}




#endif /* defined(__NES_Emulator_Go_Port__ines__) */
