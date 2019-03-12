//
//  palette.h
//  NES_Emulator_Go_Port
//
//  Created by Samuel Hunt on 19/02/2018.
//
//

#ifndef NES_Emulator_Go_Port_palette_h
#define NES_Emulator_Go_Port_palette_h


#include "../JuceLibraryCode/JuceHeader.h"


struct NesColour {
    NesColour () {
        r = g = b = 0;
        a = 0xFF;
    }
    NesColour (UInt8 _r, UInt8 _g,  UInt8 _b) {
        r = _r;
        g = _g;
        b = _b;
        a = 0xFF;
    }
    UInt8 r, g, b, a;
    
};


static NesColour Palette[64];

//var Palette [64]color.RGBA

static void palletInit ()
{
    UInt32 colours[] = {
		0x666666, 0x002A88, 0x1412A7, 0x3B00A4, 0x5C007E, 0x6E0040, 0x6C0600, 0x561D00,
		0x333500, 0x0B4800, 0x005200, 0x004F08, 0x00404D, 0x000000, 0x000000, 0x000000,
		0xADADAD, 0x155FD9, 0x4240FF, 0x7527FE, 0xA01ACC, 0xB71E7B, 0xB53120, 0x994E00,
		0x6B6D00, 0x388700, 0x0C9300, 0x008F32, 0x007C8D, 0x000000, 0x000000, 0x000000,
		0xFFFEFF, 0x64B0FF, 0x9290FF, 0xC676FF, 0xF36AFF, 0xFE6ECC, 0xFE8170, 0xEA9E22,
		0xBCBE00, 0x88D800, 0x5CE430, 0x45E082, 0x48CDDE, 0x4F4F4F, 0x000000, 0x000000,
		0xFFFEFF, 0xC0DFFF, 0xD3D2FF, 0xE8C8FF, 0xFBC2FF, 0xFEC4EA, 0xFECCC5, 0xF7D8A5,
		0xE4E594, 0xCFEF96, 0xBDF4AB, 0xB3F3CC, 0xB5EBF2, 0xB8B8B8, 0x000000, 0x000000,
	};
    
    for (int i = 0; i < 64; i++) {
        uint32 byte = colours[i];
        UInt8 r = byte >> 16;
        UInt8 g = byte >> 8;
        UInt8 b = byte;
        Palette[i] = NesColour(r, g, b);
    }

}

#endif
