//
//  ppu.cpp
//  NES_Emulator_Go_Port
//
//  Created by Samuel Hunt on 20/02/2018.
//
//

#include "ppu.h"
#include "console.h"

void PPU::print ()
{
    printf("Pallet data ---- \n");
    for (int i = 0; i < 32; i++) {
        printf("%i ", paletteData[i]);
        if (i % 8 == 7) {
            printf("\n");
        }
    }
    
    printf("OAM data ---- \n");

    for (int i = 0; i < 256; i++) {
        printf("%i ", oamData[i]);
        if (i % 16 == 15) {
            printf("\n");
        }
    }
    
    printf("Name table data ---- \n");

    for (int i = 0; i < 2048; i++) {
        printf("%i ", nameTableData[i]);
        if (i % 32 == 31) {
            printf("\n");
        }
    }
    
}


PPU::PPU (Console * _console) : memory(_console)
{
    console = _console;

    //ppu := PPU{Memory: NewPPUMemory(console), console: console}
    
    for (int i = 0; i < pixelWidth; i++) {
        for (int j = 0; j < pixelHeight; j++) {
            front[i][j] = NesColour(0, 0, 0);
            back[i][j] = NesColour(0, 0, 0);
        }
    }
    
    Cycle = 0;
    ScanLine = 0;
    Frame = 0;
    for (int i = 0; i < ePaletteSize; i++) {
        paletteData[i] = 0;
    }
    for (int i = 0; i < eNameTableSize; i++) {
        nameTableData[i] = 0;
    }
    for (int i = 0; i < eNumOfSprite; i++) {
        oamData[i] = {0,0,0,0};
    }
    
//    vramAddress = tempAddress = {0,0,0,0};
    
    x = w = f = 0;
    _register = 0;

    nmiOccurred = nmiOutput = nmiPrevious = nmiDelay = 0;
    
	nameTableByte = attributeTableByte = lowTileByte = highTileByte = 0;
	tileData = 0;
    
	// sprite temporary variables
	spriteCount = 0;
    
    for (int i = 0; i < 8; i++) {
        spritePatterns[i] = 0;
        spritePositions[i] = 0;
        spritePriorities[i] = 0;
        spriteIndexes[i] = 0;
    }
    
    
	flagNameTable =  flagIncrement = flagSpriteTable = flagBackgroundTable = flagBackgroundTable = flagSpriteSize = flagMasterSlave = 0;

    flagMasterSlave = 0; // 0: read EXT; 1: write EXT
    
    flagGrayscale = flagShowLeftBackground = flagShowLeftSprites = flagShowBackground = flagShowSprites = flagRedTint = flagGreenTint = flagBlueTint = 0;
    

    flagSpriteZeroHit = 0;
	flagSpriteOverflow = 0;


	oamAddress = 0;
    

	bufferedData = 0; // for buffered reads
    
    
    
    reset();
    palletInit();
    
    disableTileMap = disableSprites = false;
}

void PPU::reset() {
    Cycle = 340;
    ScanLine = 240;
    Frame = 0;
    writeControl(0);
    writeMask(0);
    writeOAMAddress(0);
}


Byte PPU::readPalette(UInt16 address)  {
    if (address >= 16 && (address%4) == 0) {
        address -= 16;
    }
    return paletteData[address];
}

void PPU::writePalette(UInt16 address, Byte value) {
    if (address >= 16 && (address%4) == 0) {
        address -= 16;
    }
    jassert(address < 32);
    paletteData[address] = value;
}


Byte PPU::readRegister( UInt16 address) {
    
    switch (address) {
        case eRegisters::eStatus:
            return readStatus();
            break;
        case eRegisters::eOAMReadWrite:
            return readOAMData();
            break;
        case eRegisters::eData:
            return readData();
            break;
    }
    return 0;
    
}

void PPU::writeRegister(UInt16 address, Byte value) {
    
    _register = value;
    switch (address) {
        case eRegisters::eControl:
            writeControl(value);
            break;
        case eRegisters::eMask:
            writeMask(value);
            break;
        case eRegisters::eOAMAddress:
            writeOAMAddress(value);
            break;
        case eRegisters::eOAMReadWrite:
            writeOAMData(value);
            break;
        case eRegisters::eScroll:
            writeScroll(value);
            break;
        case eRegisters::ePPUReadWrite:
            writeAddress(value);
            break;
        case eRegisters::eData:
            writeData(value);
            break;
        case 0x4014:
            writeDMA(value);
            break;
        default:
            jassertfalse;
    }

}



// $2000: PPUCTRL
void PPU::writeControl(Byte value) {
    flagNameTable = (value >> 0) & 3;
    flagIncrement = (value >> 2) & 1;
    flagSpriteTable = (value >> 3) & 1;
    flagBackgroundTable = (value >> 4) & 1;
    flagSpriteSize = (value >> 5) & 1;
    flagMasterSlave = (value >> 6) & 1;
    
    nmiOutput = ((value>>7)&1) == 1;
    
    //see if NMI needs to happen
    nmiChange();
    // t: ....BA.. ........ = d: ......BA
    UInt16 value16 = value;
    // the bottom two bits are the nameTable
    
    tempAddress.setRaw((tempAddress.getRaw() & 0xF3FF) | (( value16 & 0x03) << 10));
	// t: ....BA.. ........ = d: ......BA
}

// $2001: PPUMASK
void PPU::writeMask(Byte value) {
    flagGrayscale = (value >> 0) & 1;
    flagShowLeftBackground = (value >> 1) & 1;
    flagShowLeftSprites = (value >> 2) & 1;
    flagShowBackground = (value >> 3) & 1;
    flagShowSprites = (value >> 4) & 1;
    flagRedTint = (value >> 5) & 1;
    flagGreenTint = (value >> 6) & 1;
    flagBlueTint = (value >> 7) & 1;
}

// $2002: PPUSTATUS
Byte PPU::readStatus() {
    Byte result = _register & 0x1F;
    result |= (flagSpriteOverflow << 5);
    result |= (flagSpriteZeroHit << 6);
    if (nmiOccurred) {
        result |= (1 << 7);
    }
    nmiOccurred = false;
    nmiChange();
    // w:                   = 0
    w = 0;
    return result;
    
    
}

// $2003: OAMADDR
void PPU::writeOAMAddress(Byte value) {
    oamAddress = value;
}

// $2004: OAMDATA (read)
Byte PPU::readOAMData() {
    const Byte sprite = oamAddress/4;
    const Byte spriteData = oamAddress%4;
    return oamData[sprite].access[spriteData];
}

// $2004: OAMDATA (write)
void PPU::writeOAMData(Byte value) {
    const Byte sprite = oamAddress/4;
    const Byte spriteData = oamAddress%4;
    oamData[sprite].access[spriteData] = value;
    oamAddress++;
}

// $2005: PPUSCROLL
void PPU::writeScroll(Byte value) {
    UInt16 value16 = value;

    if (w == 0) {
        // t: ........ ...HGFED = d: HGFED...
        // x:               CBA = d: .....CBA
        // w:                   = 1
        tempAddress.setRaw((tempAddress.getRaw() & 0xFFE0) | (value16 >> 3));
        x = value & 0x07;
        w = 1;
    } else {
        // t: .CBA..HG FED..... = d: HGFEDCBA
        // w:                   = 0
        tempAddress.setRaw((tempAddress.getRaw() & 0x8FFF) | (( value16 & 0x07) << 12));
        tempAddress.setRaw((tempAddress.getRaw() & 0xFC1F) | (( value16 & 0xF8) << 2));
        w = 0;
    }
}

// $2006: PPUADDR
void PPU::writeAddress(Byte value) {
    if (w == 0) {
        // t: ..FEDCBA ........ = d: ..FEDCBA
        // t: .X...... ........ = 0
        // w:                   = 1
        tempAddress.setRaw((tempAddress.getRaw() & 0x80FF) | ((( (UInt16)  value) & 0x3F) << 8));
        w = 1;
    } else {
        // t: ........ HGFEDCBA = d: HGFEDCBA
        // v                    = t
        // w:                   = 0
        tempAddress.setRaw((tempAddress.getRaw() & 0xFF00) | ( (UInt16)  value));
        vramAddress = tempAddress;
        w = 0;
    }
}
// $2007: PPUDATA (read)
Byte  PPU::readData ()
{
    Byte value = memory.read(vramAddress.getRaw());
    // emulate buffered reads
    if (vramAddress.getRaw()%0x4000 < 0x3F00) {
        Byte buffered = bufferedData;
        bufferedData = value;
        value = buffered;
    } else {
        bufferedData = memory.read(vramAddress.getRaw() - 0x1000);
    }
    // increment address
    if (flagIncrement == 0) {
        vramAddress.setRaw(vramAddress.getRaw() + 1);
    } else {
        vramAddress.setRaw(vramAddress.getRaw() + 32);
    }
    return value;
}

// $2007: PPUDATA (write)
void PPU::writeData(Byte value) {
    memory.write(vramAddress.getRaw(), value);
    if (flagIncrement == 0) {
        vramAddress.setRaw(vramAddress.getRaw() + 1);
    } else {
        vramAddress.setRaw(vramAddress.getRaw() + 32);
    }
}

// $4014: OAMDMA
void PPU::writeDMA(Byte value) {
    //        cpu := ppu.console.CPU
    UInt16 value16 = value;
    UInt16 address = value16 << 8;
    for (int i = 0; i < 256; i++) {
        oamData[i/4].access[i%4] = console->cpu.memory.read(address);
#warning FIX HERE
        oamAddress++;
        address++;
    }
    console->cpu.stall += 513;
    if (console->cpu.Cycles%2 == 1) {
        console->cpu.stall++;
    }
}


void PPU::incrementX() {
    // increment hori(v)
    // if coarse X == 31
#if 1
    if ((vramAddress.xCoarseScroll) == 31) {
        // coarse X = 0
        vramAddress.xCoarseScroll = 0;
        // switch horizontal nametable
        vramAddress.nameTableSelect ^= 0x1;
//        vramAddress.setRaw(vramAddress.x ^ 0x0400);
    } else {
        // increment coarse X
        vramAddress.xCoarseScroll++;
    }
#endif

    
//    if ((vramAddress.getRaw() & 0x001F) == 31) {
//        vramAddress.setRaw(vramAddress.getRaw() & 0xFFE0);
//
//        vramAddress.setRaw(vramAddress.getRaw() ^ 0x0400);
//
//    }
//    else {
//        vramAddress.setRaw(vramAddress.getRaw() + 1);
//    }
}

void PPU::incrementY() {
    // increment vert(v)
    // if fine Y < 7
    if ((vramAddress.yFineScroll) != 7) {
        // increment fine Y
        vramAddress.yFineScroll++;
    } else {
        // fine Y = 0
        vramAddress.yFineScroll = 0;

        // let y = coarse Y
        const int y = vramAddress.yCoarseScroll;
        if (y == 29) {
            // coarse Y = 0
            vramAddress.yCoarseScroll = 0;
            // switch vertical nametable
            vramAddress.nameTableSelect ^= 0x2;

        } else if (y == 31) {
            // coarse Y = 0, nametable not switched
            vramAddress.yCoarseScroll = 0;
        } else {
            // increment coarse Y
            vramAddress.yCoarseScroll++;
        }
        // put coarse Y back into v
    }
}

void PPU::copyX() {
    // hori(v) = hori(t)
    // v: .....F.. ...EDCBA = t: .....F.. ...EDCBA
//    v = (v & 0xFBE0) | (t & 0x041F);

    vramAddress.setRaw((vramAddress.getRaw() & 0xFBE0) | (tempAddress.getRaw() & 0x041F));

}

void PPU::copyY() {
    // vert(v) = vert(t)
    // v: .IHGF.ED CBA..... = t: .IHGF.ED CBA.....
    vramAddress.setRaw((vramAddress.getRaw() & 0x841F) | (tempAddress.getRaw() & 0x7BE0));
}

void PPU::nmiChange() {
    bool nmi = nmiOutput && nmiOccurred;
    if (nmi && !nmiPrevious) {
        // TODO: this fixes some games but the delay shouldn't have to be so
        // long, so the timings are off somewhere
        nmiDelay = 15;
    }
    nmiPrevious = nmi;
}
void PPU::setVerticalBlank() {
//    for (int i = 0; i < pixelWidth; i++) {
//        for (int j = 0; j < pixelHeight; j++) {
//            NesColour swap = front[i][j];
//            front[i][j] = back[i][j];
//            back[i][j] = swap;
//        }
//    }
    
    for (int x = 0; x < pixelWidth; x++) {
        for (int y = 0; y < pixelHeight; y++) {
            NesColour swap = front[x][y];
            front[x][y] = back[x][y];
            back[x][y] = swap;
        }

    }
    
    nmiOccurred = true;
    nmiChange();
}


void PPU::clearVerticalBlank() {
    nmiOccurred = false;
    nmiChange();
}

void PPU::fetchNameTableByte() {
    UInt16 localV = vramAddress.getRaw();
    UInt16 address = 0x2000 | (localV & 0x0FFF);
    nameTableByte = memory.read(address);
}

void PPU::fetchAttributeTableByte() {
    //im not sure why there is a copy here...
    UInt16 localV = vramAddress.getRaw();
    UInt16 address = 0x23C0 | (localV & 0x0C00) | ((localV >> 4) & 0x38) | ((localV >> 2) & 0x07);
    UInt16 shift = ((localV >> 4) & 4) | (localV & 2);
    attributeTableByte = ((memory.read(address) >> shift) & 3) << 2;
}
void PPU::fetchLowTileByte() {
    UInt16 fineY = (vramAddress.getRaw() >> 12) & 7;
    UInt16 table = flagBackgroundTable;
    UInt16 tile = nameTableByte;
    UInt16 address = (0x1000*table) + tile*16 + fineY;
    lowTileByte = memory.read(address);
}

void PPU::fetchHighTileByte() {
    UInt16 fineY = (vramAddress.getRaw() >> 12) & 7;
    UInt16 table = flagBackgroundTable;
    UInt16 tile = nameTableByte;
    UInt16 address = 0x1000*table + tile*16 + fineY;
    highTileByte = memory.read(address + 8);
}


void PPU::storeTileData() {
    UInt32 data = 0;
    for (int i = 0; i < 8; i++) {
        UInt32 a = attributeTableByte;
        UInt32 p1 = (lowTileByte & 0x80) >> 7;
        UInt32 p2 = (highTileByte & 0x80) >> 6;
        lowTileByte = lowTileByte << 1;
        highTileByte = highTileByte << 1;
        
        /*
         
        lowTileByte = 1 << lowTileByte;
        highTileByte =  1 << highTileByte;
        */
        
        data = data << 4;
        data = data | (a | p1 | p2);
    }
    tileData = tileData | data;
}

UInt32 PPU::fetchTileData() {
    return (UInt32) ( tileData >> 32);
}

Byte PPU::backgroundPixel() {
    if (flagShowBackground == 0) {
        return 0;
    }
    Byte data = fetchTileData() >> ((7 - x) * 4);
    return data & 0x0F;
}


PPU::SpritePixel PPU::spritePixel() {
    if (flagShowSprites == 0) {
        return {0, 0};
    }
    for (int i = 0; i < spriteCount; i++) {
        int offset = (Cycle - 1) - spritePositions[i];
        if (offset < 0 || offset > 7) {
            continue;
        }
        offset = 7 - offset;
//        Byte color = byte((ppu.spritePatterns[i] >> byte(offset*4)) & 0x0F)
//        color := byte((ppu.spritePatterns[i] >> byte(offset*4)) & 0x0F)
        Byte ofSum = offset * 4;
        Byte color = (spritePatterns[i] >> (offset*4)) & 0x0F;
        if ((color%4) == 0) {
            continue;
        }
//        return {(Byte)i, color};
        //return {(Byte)i, color };
        return {color, (Byte)i };

    }
    return {0, 0};
}
void PPU::renderPixel() {
    int xl = Cycle - 1;
    jassert(xl < pixelWidth);
    int yl = ScanLine;
    int background = backgroundPixel();

    
    SpritePixel spirtePix = spritePixel();

    
    if (xl < 8 && flagShowLeftBackground == 0) {
        background = 0;
    }
    if (xl < 8 && flagShowLeftSprites == 0) {
        spirtePix.i = 0;
    }
    Byte b = (background%4) != 0;
    Byte s = (spirtePix.i%4  ) != 0;
    Byte color;
    if (!b && !s) {
        color = 0;
    }
    else if (!b && s) {
        color = spirtePix.i | 0x10;
        if (disableSprites) {
            color = disableTileMap ? 0 : background;
        }
    }
    else if (b && !s) {
        color = background;
        if (disableTileMap) {
            color = 0;
        }
    }
    else {

        if (spriteIndexes[spirtePix.colour] == 0 && xl < 255) {
            flagSpriteZeroHit = 1;
        }
        if (spritePriorities[spirtePix.colour] == 0) {
            color = spirtePix.i | 0x10;
            if (disableSprites) {
                color = disableTileMap ? 0 : background;
            }
        }
        else {
            color = background;
            if (disableTileMap) {
                color = 0;
            }
        }
    }
    
    
    NesColour c = Palette[readPalette(color%64)];
    back[xl][yl] = c;
#if 0
    Byte c = Palette[readPalette(color%64)];
//    Byte c = Palette[ppu.readPalette(uint16(color))%64]
    ppu.back.SetRGBA(x, y, c)
#endif
}




UInt32 PPU::fetchSpritePattern(int i, int row ) {

    UInt16 tile = oamData[i].tileNum;
    UInt16 attributes = oamData[i].attributes;
    UInt16 address = 0;
    if (flagSpriteSize == 0) {
        if ((attributes&0x80) == 0x80) {
            row = 7 - row;
        }
        UInt16 table = flagSpriteTable;
        address = 0x1000*table + tile*16 + row;
    }
    else {
        if ((attributes&0x80) == 0x80) {
            row = 15 - row;
        }
        UInt16 table = tile & 1;
        tile &= 0xFE;
        if (row > 7) {
            tile++;
            row -= 8;
        }
        address = 0x1000*table + tile*16 + row;

//        address = 0x1000*uint16(table) + uint16(tile)*16 + uint16(row)
    }
    UInt16 a = (attributes & 3) << 2;
    UInt16 lowTileByte = memory.read(address);
    UInt16 highTileByte = memory.read(address + 8);
    UInt32 data = 0;
    for (int i = 0; i < 8; i++) {
        UInt32 p1, p2;
        if ((attributes&0x40) == 0x40) {
            p1 = (lowTileByte & 1) << 0;
            p2 = (highTileByte & 1) << 1;
            lowTileByte = lowTileByte >> 1;
            highTileByte = highTileByte >> 1;
        } else {
            p1 = (lowTileByte & 0x80) >> 7;
            p2 = (highTileByte & 0x80) >> 6;
            lowTileByte = lowTileByte << 1;
            highTileByte = highTileByte << 1;
        }
#warning CHECK ABOVE
        data = data << 4;
        UInt32 a1 = a;

        
        data |= (a1 | p1 | p2);// ((UInt32) a) |  ((UInt32) p1) | ((UInt32) p2);
//            data = (a1 | p1 | p2);// ((UInt32) a) |  ((UInt32) p1) | ((UInt32) p2);
    }
    return data;
}



void PPU::evaluateSprites() {
    int h;
    if (flagSpriteSize == 0) {
        h = 8;
    } else {
        h = 16;
    }
    int count = 0;
    for (int i = 0; i < eNumOfSprite; i++) {
        Byte y = oamData[i].yPos;
        Byte a = oamData[i].attributes;
        Byte x = oamData[i].xPos;
        int row = ScanLine - ((int) y);
        if (row < 0 || row >= h) {
            continue;
        }
        if (count < 8) {
            spritePatterns[count] = fetchSpritePattern(i, row);
            spritePositions[count] = x;
            //jassert(x > 100);
            spritePriorities[count] = (a >> 5) & 1;
            spriteIndexes[count] = ((Byte)i);
        }
        count++;
    }
    if (count > 8) {
        count = 8;
        flagSpriteOverflow = 1;
    }
    spriteCount = count;
}

// tick updates Cycle, ScanLine and Frame counters
void PPU::tick() {
    if (nmiDelay > 0) {
        nmiDelay--;
        if (nmiDelay == 0 && nmiOutput && nmiOccurred) {
            console->cpu.triggerNMI();
        }
    }
    /* This has been added! */
//    if (nmiOutput && nmiOccurred) {
//        console->cpu.triggerNMI();
//    }
    
    if (flagShowBackground != 0 || flagShowSprites != 0) {
        if (f == 1 && ScanLine == eNumOfScanLines-1 && Cycle == 339) {
            Cycle = 0;
            ScanLine = 0;
            Frame++;
            f ^= 1;
            return;
        }
    }
    Cycle++;
    if (Cycle > 340) {
        Cycle = 0;
        ScanLine++;
        if (ScanLine > 261) {
            ScanLine = 0;
            Frame++;
            f = f ^ 1;
        }
    }
    //printf("x = %i : y = %i \n", Cycle, ScanLine);
}

// Step executes a single PPU cycle
void PPU::Step() {
    tick();
    
    bool renderingEnabled = flagShowBackground != 0 || flagShowSprites != 0;
    bool preLine = ScanLine == 261;
    bool visibleLine = ScanLine < 240;
    // postLine := ppu.ScanLine == 240
    bool renderLine = preLine || visibleLine;
    bool preFetchCycle = Cycle >= 321 && Cycle <= 336;
    bool visibleCycle = Cycle >= 1 && Cycle <= 256;
    bool fetchCycle = preFetchCycle || visibleCycle;
    
    // background logic
    if (renderingEnabled) {
        if (visibleLine && visibleCycle) {
            renderPixel();
        }
        if (renderLine && fetchCycle) {
            tileData <<= 4;// tileData << 4;
            switch (Cycle % 8) {
            case 1:
                    fetchNameTableByte();
                    break;
            case 3:
                    fetchAttributeTableByte();
                    break;
            case 5:
                    fetchLowTileByte();
                    break;
            case 7:
                    fetchHighTileByte();
                    break;
            case 0:
                    storeTileData();
                    break;
            }
        }
        if (preLine && Cycle >= 280 && Cycle <= 304) {
            copyY();
        }
        if (renderLine) {
            if (fetchCycle && Cycle%8 == 0) {
                incrementX();
            }
            if (Cycle == 256) {
                incrementY();
            }
            if (Cycle == 257) {
                copyX();
            }
        }
    }
    
    // sprite logic
    if (renderingEnabled) {
        if (Cycle == 257) {
            if (visibleLine) {
                evaluateSprites();
            } else {
                spriteCount = 0;
            }
        }
    }
    
    // vblank logic
    if (ScanLine == 241 && Cycle == 1) {
        setVerticalBlank();
    }
    if (preLine && Cycle == 1) {
        clearVerticalBlank();
        flagSpriteZeroHit = 0;
        flagSpriteOverflow = 0;
    }
}
