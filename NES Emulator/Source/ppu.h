//
//  ppu.h
//  NES_Emulator_Go_Port
//
//  Created by Samuel Hunt on 20/02/2018.
//
//

#ifndef __NES_Emulator_Go_Port__ppu__
#define __NES_Emulator_Go_Port__ppu__

#include "../JuceLibraryCode/JuceHeader.h"
#include "Memory.h"
#include "palette.h"

const int pixelWidth = 256;
const int pixelHeight = 240;

class Console;
class PPU {
public:
    union Sprite { //Each sprite contains 4 bytes
        struct {
            Byte yPos;
            Byte tileNum;
            Byte attributes;
            Byte xPos;
        };
        Byte access[4]; //allows quick access during oam read/write
    };
    struct VRamAddress {
        
        Byte xCoarseScroll : 5;
        Byte yCoarseScroll : 5;
        Byte nameTableSelect : 2;
        Byte yFineScroll : 3;
        
        uint16 getRaw ()
        {
            return xCoarseScroll | (yCoarseScroll << 5) | (nameTableSelect << 10) | (yFineScroll << 12);
        }
        void setRaw (uint16 raw)
        {
            xCoarseScroll = raw & 0x1F;
            yCoarseScroll = (raw >> 5) & 0x1F;
            nameTableSelect = (raw >> 10) & 0x3;
            yFineScroll = (raw >> 12) & 0x7;
        }
        
    };
    enum eConsants {
        eNumOfSprite = 64,
        eNumOfScanLines = 262,
        ePaletteSize = 32,
        eNameTableSize = 2048,
        ePixelMaskLeft = 8,
    };
    enum eRegisters {
        eControl = 0x2000,
        eMask,
        eStatus,
        eOAMAddress, //0x2003
        eOAMReadWrite,
        eScroll,
        ePPUReadWrite, //
        eData,
    };
    
    
    PPU (Console * _console);
    void reset();
    
    void print ();
    
    Byte readPalette(UInt16 address);
    void writePalette(UInt16 address, Byte value);
    
    Byte readRegister( UInt16 address);
    void writeRegister(UInt16  address, Byte value);
    
    
    
    
    // $2000: PPUCTRL bit masked values
    void writeControl(Byte value);
    // $2001: PPUMASK
    void writeMask(Byte value);
    
    // $2002: PPUSTATUS
    Byte readStatus();
    
    
    // $2003: OAMADDR
    void writeOAMAddress(Byte value);
    
    // $2004: OAMDATA (write)
    void writeOAMData(Byte value);
    // $2004: OAMDATA (read)
    Byte readOAMData();
    


    
    // $2005: PPUSCROLL
    void writeScroll(Byte value);
    
    // $2006: PPUADDR
    void writeAddress(Byte value);

    // $2007: PPUDATA (read)
    Byte readData ();
    // $2007: PPUDATA (write)
    void writeData(Byte value);
    
    // $4014: OAMDMA
    void writeDMA(Byte value);
    
    
    // NTSC Timing Helper Functions
    
    void incrementX();
    void incrementY() ;
    
    void copyX();
    
    void copyY();
    
    void nmiChange();
    
    void setVerticalBlank();
    
    void clearVerticalBlank();
    
    void fetchNameTableByte();
    
    void fetchAttributeTableByte();
    
    void fetchLowTileByte();
    
    void fetchHighTileByte();


    
    
    void storeTileData();
    UInt32 fetchTileData();
    
    Byte backgroundPixel();
    
    struct SpritePixel
    {
        Byte i, colour;
    };
    
    SpritePixel spritePixel();
    
    
    void renderPixel();
    
    
    
    UInt32 fetchSpritePattern(int i, int row );
    void evaluateSprites();
    // tick updates Cycle, ScanLine and Frame counters
    void tick();
    // Step executes a single PPU cycle
    void Step();     
    

    
    
    ppuMemory memory;

	Console *console; // reference to parent object
    
	int     Cycle;// 0-340
	int     ScanLine;// 0-261, 0-239=visible, 240=post, 241-260=vblank, 261=pre
	uint64  Frame;// frame counter
    
	// storage variables
	Byte paletteData[ePaletteSize];
	Byte nameTableData[eNameTableSize];
    
	Sprite oamData[eNumOfSprite];

    NesColour   front[pixelWidth][pixelHeight];
    NesColour   back[pixelWidth][pixelHeight];
    
    struct FullScreenDebug { //warpped up like
        NesColour   fullMapRender[pixelWidth*2][pixelHeight*2];
    };
    
    void fillDebugScreenBuffer (FullScreenDebug * buffer);
    

//	front         *image.RGBA
//	back          *image.RGBA
    
	// PPU registers
	VRamAddress vramAddress; // current vram address (15 bit)
	VRamAddress tempAddress; // temporary vram address (15 bit)
	Byte x;   // fine x scroll (3 bit)
	Byte writeToggle;   // write toggle (1 bit)
	Byte f;   // even/odd frame flag (1 bit)
    
	Byte _register;
    
	// NMI flags
	bool nmiOccurred;
	bool nmiOutput;
	bool nmiPrevious;
	Byte nmiDelay;
    
	// background temporary variables
	Byte nameTableByte;
	Byte attributeTableByte;
    Byte lowTileByte;
	Byte highTileByte;
	uint64 tileData;
    
	// sprite temporary variables
	int spriteCount;
	UInt32 spritePatterns[8];
	Byte spritePositions[8];
	Byte spritePriorities[8];
	Byte spriteIndexes[8];
    
	// $2000 PPUCTRL
	Byte flagNameTable; // 0: $2000; 1: $2400; 2: $2800; 3: $2C00
	Byte flagIncrement; // 0: add 1; 1: add 32
	Byte flagSpriteTable; // 0: $0000; 1: $1000; ignored in 8x16 mode
	Byte flagBackgroundTable; // 0: $0000; 1: $1000
	Byte flagSpriteSize; // 0: 8x8; 1: 8x16
	Byte flagMasterSlave; // 0: read EXT; 1: write EXT
    
	// $2001 PPUMASK
	Byte flagGrayscale; // 0: color; 1: grayscale
	Byte flagShowLeftBackground; // 0: hide; 1: show - hides the left 8 pixels
	Byte flagShowLeftSprites; // 0: hide; 1: show
	Byte flagShowBackground; // 0: hide; 1: show
	Byte flagShowSprites; // 0: hide; 1: show
	Byte flagRedTint; // 0: normal; 1: emphasized
	Byte flagGreenTint; // 0: normal; 1: emphasized
	Byte flagBlueTint; // 0: normal; 1: emphasized
    
	// $2002 PPUSTATUS
	Byte flagSpriteZeroHit;
	Byte flagSpriteOverflow;
    
	// $2003 OAMADDR
	Byte oamAddress;
    
	// $2007 PPUDATA
	Byte bufferedData; // for buffered reads
    
    bool disableTileMap;
    bool disableSprites;
};


#if 0

func (ppu *PPU) Save(encoder *gob.Encoder) error {
	encoder.Encode(ppu.Cycle)
	encoder.Encode(ppu.ScanLine)
	encoder.Encode(ppu.Frame)
	encoder.Encode(ppu.paletteData)
	encoder.Encode(ppu.nameTableData)
	encoder.Encode(ppu.oamData)
	encoder.Encode(ppu.v)
	encoder.Encode(ppu.t)
	encoder.Encode(ppu.x)
	encoder.Encode(ppu.w)
	encoder.Encode(ppu.f)
	encoder.Encode(ppu.register)
	encoder.Encode(ppu.nmiOccurred)
	encoder.Encode(ppu.nmiOutput)
	encoder.Encode(ppu.nmiPrevious)
	encoder.Encode(ppu.nmiDelay)
	encoder.Encode(ppu.nameTableByte)
	encoder.Encode(ppu.attributeTableByte)
	encoder.Encode(ppu.lowTileByte)
	encoder.Encode(ppu.highTileByte)
	encoder.Encode(ppu.tileData)
	encoder.Encode(ppu.spriteCount)
	encoder.Encode(ppu.spritePatterns)
	encoder.Encode(ppu.spritePositions)
	encoder.Encode(ppu.spritePriorities)
	encoder.Encode(ppu.spriteIndexes)
	encoder.Encode(ppu.flagNameTable)
	encoder.Encode(ppu.flagIncrement)
	encoder.Encode(ppu.flagSpriteTable)
	encoder.Encode(ppu.flagBackgroundTable)
	encoder.Encode(ppu.flagSpriteSize)
	encoder.Encode(ppu.flagMasterSlave)
	encoder.Encode(ppu.flagGrayscale)
	encoder.Encode(ppu.flagShowLeftBackground)
	encoder.Encode(ppu.flagShowLeftSprites)
	encoder.Encode(ppu.flagShowBackground)
	encoder.Encode(ppu.flagShowSprites)
	encoder.Encode(ppu.flagRedTint)
	encoder.Encode(ppu.flagGreenTint)
	encoder.Encode(ppu.flagBlueTint)
	encoder.Encode(ppu.flagSpriteZeroHit)
	encoder.Encode(ppu.flagSpriteOverflow)
	encoder.Encode(ppu.oamAddress)
	encoder.Encode(ppu.bufferedData)
	return nil
}

func (ppu *PPU) Load(decoder *gob.Decoder) error {
	decoder.Decode(&ppu.Cycle)
	decoder.Decode(&ppu.ScanLine)
	decoder.Decode(&ppu.Frame)
	decoder.Decode(&ppu.paletteData)
	decoder.Decode(&ppu.nameTableData)
	decoder.Decode(&ppu.oamData)
	decoder.Decode(&ppu.v)
	decoder.Decode(&ppu.t)
	decoder.Decode(&ppu.x)
	decoder.Decode(&ppu.w)
	decoder.Decode(&ppu.f)
	decoder.Decode(&ppu.register)
	decoder.Decode(&ppu.nmiOccurred)
	decoder.Decode(&ppu.nmiOutput)
	decoder.Decode(&ppu.nmiPrevious)
	decoder.Decode(&ppu.nmiDelay)
	decoder.Decode(&ppu.nameTableByte)
	decoder.Decode(&ppu.attributeTableByte)
	decoder.Decode(&ppu.lowTileByte)
	decoder.Decode(&ppu.highTileByte)
	decoder.Decode(&ppu.tileData)
	decoder.Decode(&ppu.spriteCount)
	decoder.Decode(&ppu.spritePatterns)
	decoder.Decode(&ppu.spritePositions)
	decoder.Decode(&ppu.spritePriorities)
	decoder.Decode(&ppu.spriteIndexes)
	decoder.Decode(&ppu.flagNameTable)
	decoder.Decode(&ppu.flagIncrement)
	decoder.Decode(&ppu.flagSpriteTable)
	decoder.Decode(&ppu.flagBackgroundTable)
	decoder.Decode(&ppu.flagSpriteSize)
	decoder.Decode(&ppu.flagMasterSlave)
	decoder.Decode(&ppu.flagGrayscale)
	decoder.Decode(&ppu.flagShowLeftBackground)
	decoder.Decode(&ppu.flagShowLeftSprites)
	decoder.Decode(&ppu.flagShowBackground)
	decoder.Decode(&ppu.flagShowSprites)
	decoder.Decode(&ppu.flagRedTint)
	decoder.Decode(&ppu.flagGreenTint)
	decoder.Decode(&ppu.flagBlueTint)
	decoder.Decode(&ppu.flagSpriteZeroHit)
	decoder.Decode(&ppu.flagSpriteOverflow)
	decoder.Decode(&ppu.oamAddress)
	decoder.Decode(&ppu.bufferedData)
	return nil
}
#endif

#endif /* defined(__NES_Emulator_Go_Port__ppu__) */
