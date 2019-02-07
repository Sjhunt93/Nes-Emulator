//
//  cartridge.h
//  NES_Emulator_Go_Port
//
//  Created by Samuel Hunt on 19/02/2018.
//
//

#ifndef NES_Emulator_Go_Port_cartridge_h
#define NES_Emulator_Go_Port_cartridge_h

#include "types.h"

class Cartridge {
public:
    
    
    Cartridge ()
    {
        SRAM = new Byte[0x2000];
        PRG = nullptr;
        CHR = nullptr;
    }
    ~Cartridge ()
    {
        delete [] PRG;
        delete [] CHR;
        delete [] SRAM;
    }

    Cartridge (const Cartridge & other)
    {
//        delete [] PRG;
//        delete [] CHR;
//        delete [] SRAM;
        
        NumPRG = other.NumPRG;
        NumCHR = other.NumCHR;
        SRAM = new Byte[0x2000];

        const int prgSize = other.NumPRG * 16384;
        const int chrSize = other.NumCHR * 8192;

        prgAlloc = prgSize;
        chrAlloc = chrSize;
        // * 8192;
        
        PRG = new Byte[prgSize];
        CHR = new Byte[chrSize];
        
        for (int i = 0; i < prgSize; i++) {
            PRG[i] = other.PRG[i];
        }
        for (int i = 0; i < chrSize; i++) {
            CHR[i] = other.CHR[i];
        }
        
        Mapper = other.Mapper;
        Mirror = other.Mirror;
        Battery = other.Battery;
        
    }
    
    Byte *      PRG; // PRG-ROM banks
    Byte *      CHR; // CHR-ROM banks
    Byte *      SRAM; // Save RAM
	Byte        Mapper; // mapper type
	Byte        Mirror;  // mirroring mode
	Byte        Battery; // battery present
    
    Byte NumPRG;    // number of PRG-ROM banks (16KB each)
	Byte NumCHR;       // number of CHR-ROM banks (8KB each)
    
    
    int prgAlloc;
    int chrAlloc;
    
private:
    
};

/*
 
 
 func (cartridge *Cartridge) Save(encoder *gob.Encoder) error {
 encoder.Encode(cartridge.PRG)
 encoder.Encode(cartridge.CHR)
 encoder.Encode(cartridge.SRAM)
 encoder.Encode(cartridge.Mirror)
 return nil
 }
 
 func (cartridge *Cartridge) Load(decoder *gob.Decoder) error {
 decoder.Decode(&cartridge.PRG)
 decoder.Decode(&cartridge.CHR)
 decoder.Decode(&cartridge.SRAM)
 decoder.Decode(&cartridge.Mirror)
 return nil
 }

 
 */



#endif
