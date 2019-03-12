 //
//  cpu.cpp
//  NES_Emulator_Go_Port
//
//  Created by Samuel Hunt on 19/02/2018.
//
//

#include "cpu.h"
#include "Console.h"


CPU::CPU (Console * console) : memory(console)
{
    //	cpu := CPU{Memory: NewCPUMemory(console)}
    
    //    cpu.memory.createTable();
    //    cpu.memory.Reset()
    //        CPU * cpu;
    
    //        return cpu;
    
    
//    cpuMemory =
    /*
    Cycles = 0; // number of cycles
	PC = 0; // program counter
	SP = 0;   // stack pointer
     */
    
    A = X = Y = C = Z = I = D = B = U = V = N = 0;
    Cycles = 0;
	stall = 0;

	interrupt = 0;   // interrupt type to perform

}

void CPU::init ()
{
    stepInfo si;
    si.address = 0x80;
    si.mode = 3;
    si.pc = 10;
    map[0](this, &si);
}

void CPU::Reset()
{
    PC = read16(0xFFFC);
    SP = 0xFD;
    setFlags(0x24);
}


/*static*/ bool CPU::pagesDiffer(UInt16 a, UInt16 b) {
    return (a&0xFF00) != (b&0xFF00);
}

// addBranchCycles adds a cycle for taking a branch and adds another cycle
// if the branch jumps to a new page
void CPU::addBranchCycles(stepInfo info) {
    Cycles++;
    if (pagesDiffer(info.pc, info.address)) {
        Cycles++;
    }
}

void CPU::compare(Byte a, Byte b) {
    setZN(a - b);
    if (a >= b) {
        C = 1;
    } else {
        C = 0;
    }
}

UInt16 CPU::read16(UInt16 address)  {
    UInt16 lo = memory.read(address);
    UInt16 hi = memory.read(address+1);
    return (hi<<8) | lo;
}
UInt16 CPU::read16Bug(UInt16 address)
{
    UInt16 a = address;
    Byte bug = a+1;
    UInt16 b = (a & 0xFF00) | ((UInt16)bug);// uint16(byte(a)+1)
    UInt16 lo = memory.read(a);
    UInt16 hi = memory.read(b);
    return ((hi<<8) | lo);
    
}

void CPU::push (Byte value)
{
    memory.write(0x100| ((UInt16) SP), value);
    SP--;
}
Byte CPU::pull ()
{
    SP++;
    return memory.read(0x100 | ((UInt16) SP));
}

void CPU::push16 (UInt16 value)
{
    Byte hi = (value >> 8);
    Byte lo = (value & 0xFF);
    push(hi);
    push(lo);
}
UInt16 CPU::pull16 ()
{
    UInt16 lo = pull();
    UInt16 hi = pull();
    return ((hi<<8) | lo);
}

Byte CPU::flags ()
{
    Byte flags =0;
    flags |= C << 0;
    flags |= Z << 1;
    flags |= I << 2;
    flags |= D << 3;
    flags |= B << 4;
    flags |= U << 5;
    flags |= V << 6;
    flags |= N << 7;
    return flags;
}
void CPU::setFlags (Byte flags)
{
    C = (flags >> 0) & 1;
    Z = (flags >> 1) & 1;
    I = (flags >> 2) & 1;
    D = (flags >> 3) & 1;
    B = (flags >> 4) & 1;
    U = (flags >> 5) & 1;
    V = (flags >> 6) & 1;
    N = (flags >> 7) & 1;
}


void CPU::setZ (Byte value)
{
    Z = value == 0 ? 1 : 0;
}

void CPU::setN (Byte value)
{
    if ((value&0x80) != 0) {
        N = 1;
    } else {
        N = 0;
    }
}

void CPU::setZN (Byte value)
{
    setZ(value);
    setN(value);
}

void CPU::triggerNMI ()
{
    interrupt = interruptNMI;
    
}

void CPU::triggerIRQ ()
{
    if (I == 0) {
        interrupt = interruptIRQ;
    }
}

int CPU::step()
{
    
    if (stall > 0) {
        stall--;
        return 1;
    }
    
    int64 localCycle = Cycles;
    
    if (interrupt == interruptNMI) {
        nmi();
    }
    else if (interrupt == interruptIRQ) {
        irq();
    }
    interrupt = interruptNone;
    
    int opcode = memory.read(PC);

    Byte mode = instructionModes[opcode];
    UInt16 address = 0;
    bool pageCrossed = false;
    
   // printInstruction();
//    printf("%x \n", memory.read(0x2002));
    const UInt16 X16 = X;
    const UInt16 Y16 = Y;
    
    switch (mode) {
        case modeAbsolute:
            address = read16(PC + 1);
            break;
        case modeAbsoluteX:
        {
            address = read16(PC+1) + X16;
            pageCrossed = pagesDiffer(address- X16, address);
            break;
        }
        case modeAbsoluteY:
        {
            address = read16(PC+1) + Y16;
            pageCrossed = pagesDiffer(address-Y16, address);
            break;
        }
        case modeAccumulator:
        {
            address = 0;
            break;
        }
        case modeImmediate:
        {
            address = PC + 1;
            break;
        }
        case modeImplied:
        {
            address = 0;
            break;
        }
        case modeIndexedIndirect:
        {
//            address = cpu.read16bug(uint16(cpu.Read(cpu.PC+1) + cpu.X))
            Byte b = memory.read(PC+1) + X;
            address = read16Bug(b);
            //address = read16Bug( ((UInt16) memory.read(PC+1)) + X);
            break;
        }
        case modeIndirect:
        {
            address = read16Bug(read16(PC + 1));
            break;
        }
        case modeIndirectIndexed:
        {
            address = read16Bug(memory.read(PC + 1)) + Y;
            pageCrossed = pagesDiffer(address-Y, address);
            break;
        }
        case modeRelative:
        {
            UInt16 offset = memory.read(PC + 1);
            if (offset < 0x80) {
                address = PC + 2 + offset;
            } else {
                address = (PC + 2 + offset) - 0x100;
            }
            break;
        }
        case modeZeroPage:
        {
            address = memory.read(PC + 1);
            break;
        }
        case modeZeroPageX:
        {
            address = (memory.read(PC+1)+X) & 0xff;
            break;
        }
        case modeZeroPageY:
        {
            address = (memory.read(PC+1)+Y) & 0xff;
            break;
        }
    }
    PC += instructionSizes[opcode];
    Cycles += instructionCycles[opcode];
    
    
    if (pageCrossed) {
        Cycles += instructionPageCycles[opcode];
    }

    stepInfo si = {address, PC, mode};
    map[opcode](this, &si);
    return (int) (Cycles - localCycle);
}


void CPU::printInstruction ()
{
    
    UInt16 opcode = memory.read(PC);
    int bytes = instructionSizes[opcode];
    
    String name = instructionNames[opcode];
    
    String w0 = String( memory.read(PC) );
    String w1 = bytes < 2 ? " " : String( memory.read(PC+1) );
    String w2 = bytes < 3 ? " " : String( memory.read(PC+2) );
    
    printf("%x %x %x %x %s ", PC, w0.getIntValue(), w1.getIntValue(), w2.getIntValue(), name.toRawUTF8());
    printf("A:%x X:%x Y:%x P:%x SP:%x CYC: %llu \n", A, X, Y, flags(), SP, (Cycles*3)%341);
}



void CPU::nmi ()
{
    push16(PC);

    php(this, nullptr);
    PC = read16(0xFFFA);
    I = 1;
    Cycles += 7;
}

void CPU::irq ()
{
    push16(PC);
    php(this, nullptr);
    PC = read16(0xFFFE);
    I = 1;
    Cycles += 7;
}

void CPU::adc(CPU * cpu, stepInfo * info)
{
    
    Byte a = cpu->A;
    Byte b = cpu->memory.read(info->address);
    Byte c = cpu->C;
    
    cpu->A = a + b + c;
    cpu->setZN(cpu->A);
    
    
    
    int carry = ((int) a) + ((int) b) + ((int) c);
    if (carry > 0xFF) {
        cpu->C = 1;
    }
    else {
        cpu->C = 0;
    }
    
    if (((a^b)&0x80) == 0 && ((a^cpu->A)&0x80) != 0) {
        cpu->V = 1;
    } else {
        cpu->V = 0;
    }
    
    
}

// AND - Logical AND
void CPU::And(CPU * cpu, stepInfo * info)
{
    cpu->A = cpu->A & cpu->memory.read(info->address);
    cpu->setZN(cpu->A);
}

// ASL - Arithmetic Shift Left
void CPU::asl(CPU * cpu, stepInfo * info)
{
    if (info->mode == modeAccumulator) {
        cpu->C = (cpu->A >> 7) & 1;
        cpu->A = cpu->A << 1;
        cpu->setZN(cpu->A);
    }
    else {
        Byte value = cpu->memory.read(info->address);
        cpu->C = (value >> 7) & 1;
        value = value << 1;
        cpu->memory.write(info->address, value);
        cpu->setZN(value);
    }
    
}


// BCC - Branch if Carry Clear
void CPU::bcc(CPU * cpu, stepInfo * info)
{
    if (cpu->C == 0) {
        cpu->PC = info->address;
        cpu->addBranchCycles(*info);
    }
}

// BCS - Branch if Carry Set
void CPU::bcs(CPU * cpu, stepInfo * info)
{
    if (cpu->C != 0) {
        cpu->PC = info->address;
        cpu->addBranchCycles(*info);
    }
}

// BEQ - Branch if Equal
void CPU::beq(CPU * cpu, stepInfo * info)
{
    if (cpu->Z != 0) {
        cpu->PC = info->address;
        cpu->addBranchCycles(*info);
    }
}

// BIT - Bit Test
void CPU::bit(CPU * cpu, stepInfo * info)
{
    Byte value = cpu->memory.read(info->address);
    cpu->V = (value >> 6) & 1;
    cpu->setZ(value & cpu->A);
    cpu->setN(value);
}

// BMI - Branch if Minus
void CPU::bmi(CPU * cpu, stepInfo * info)
{
    if (cpu->N != 0) {
        cpu->PC = info->address;
        cpu->addBranchCycles(*info);
    }
}

// BNE - Branch if Not Equal
void CPU::bne(CPU * cpu, stepInfo * info)
{
    if (cpu->Z == 0) {
        cpu->PC = info->address;
        cpu->addBranchCycles(*info);
    }
}
// BPL - Branch if Positive
void CPU::bpl(CPU * cpu, stepInfo * info)
{
    if (cpu->N == 0) {
        cpu->PC = info->address;
        cpu->addBranchCycles(*info);
    }
}




// BRK - Force Interrupt
void CPU::brk(CPU * cpu, stepInfo * info)
{
    cpu->push16(cpu->PC);
    cpu->php(cpu, info);
    cpu->sei(cpu, info);
    cpu->PC = cpu->read16(0xFFFE);
}

// BVC - Branch if Overflow Clear
void CPU::bvc(CPU * cpu, stepInfo * info)
{
    if (cpu->V == 0) {
        cpu->PC = info->address;
        cpu->addBranchCycles(*info);
    }
}

// BVS - Branch if Overflow Set
void CPU::bvs(CPU * cpu, stepInfo * info)
{
    if (cpu->V != 0) {
        cpu->PC = info->address;
        cpu->addBranchCycles(*info);
    }
}

void CPU::clc(CPU * cpu, stepInfo * info) {cpu->C = 0;}
void CPU::cld(CPU * cpu, stepInfo * info) { cpu->D = 0; }
void CPU::cli(CPU * cpu, stepInfo * info) { cpu->I = 0; }
void CPU::clv(CPU * cpu, stepInfo * info) { cpu->V = 0; }


// CMP - Compare
void CPU::cmp(CPU * cpu, stepInfo * info) {
    Byte value = cpu->memory.read(info->address);
    cpu->compare(cpu->A, value);
}

// CPX - Compare X Register
void CPU::cpx(CPU * cpu, stepInfo * info) {
    Byte value = cpu->memory.read(info->address);
    cpu->compare(cpu->X, value);
}
// CPY - Compare Y Register

void CPU::cpy(CPU * cpu, stepInfo * info) {
    Byte value = cpu->memory.read(info->address);
    cpu->compare(cpu->Y, value);
}

// DEC - Decrement Memory
void CPU::dec(CPU * cpu, stepInfo * info)
{
    Byte value = cpu->memory.read(info->address) - 1;
    cpu->memory.write(info->address, value);
    cpu->setZN(value);
}

// DEX - Decrement X Register
void CPU::dex(CPU * cpu, stepInfo * info)
{
    cpu->X--;
    cpu->setZN(cpu->X);
}
void CPU::dey(CPU * cpu, stepInfo * info)
{
    cpu->Y--;
    cpu->setZN(cpu->Y);
}


// EOR - Exclusive OR
void CPU::eor(CPU * cpu, stepInfo * info)
{
    cpu->A = cpu->A ^ cpu->memory.read(info->address);
    cpu->setZN(cpu->A);
}

// INC - Increment Memory
void CPU::inc(CPU * cpu, stepInfo * info)
{
    Byte value = cpu->memory.read(info->address) + 1;
    cpu->memory.write(info->address, value);
    cpu->setZN(value);
}

// INX - Increment X Register
void CPU::inx(CPU * cpu, stepInfo * info)
{
    cpu->X++;
    cpu->setZN(cpu->X);
}

// INY - Increment Y Register
void CPU::iny(CPU * cpu, stepInfo * info)
{
    cpu->Y++;
    cpu->setZN(cpu->Y);
}

// JMP - Jump
void CPU::jmp(CPU * cpu, stepInfo * info)
{
    cpu->PC = info->address;
}

// JSR - Jump to Subroutine
void CPU::jsr(CPU * cpu, stepInfo * info)
{
    cpu->push16(cpu->PC - 1);
    cpu->PC = info->address;
}

// LDA - Load Accumulator
void CPU::lda(CPU * cpu, stepInfo * info)
{
    cpu->A = cpu->memory.read(info->address);
    cpu->setZN(cpu->A);
}

// LDX - Load X Register
void CPU::ldx(CPU * cpu, stepInfo * info)
{
    cpu->X = cpu->memory.read(info->address);
    cpu->setZN(cpu->X);
}

// LDY - Load Y Register
void CPU::ldy(CPU * cpu, stepInfo * info)
{
    cpu->Y = cpu->memory.read(info->address);
    cpu->setZN(cpu->Y);
}

// LSR - Logical Shift Right
void CPU::lsr(CPU * cpu, stepInfo * info)
{
    if (info->mode == modeAccumulator) {
        cpu->C = cpu->A & 1;
        cpu->A = cpu->A >> 1;
        cpu->setZN(cpu->A);
    } else {
        Byte value = cpu->memory.read(info->address);
        cpu->C = value & 1;
        value = value >> 1;
        cpu->memory.write(info->address, value);
        cpu->setZN(value);
    }
}

// NOP - No Operation
void CPU::nop(CPU * cpu, stepInfo * info)
{
    
}
// ORA - Logical Inclusive OR
void CPU::ora(CPU * cpu, stepInfo * info) {
    cpu->A = cpu->A | cpu->memory.read(info->address);
    cpu->setZN(cpu->A);
}

// PHA - Push Accumulator
void CPU::pha(CPU * cpu, stepInfo * info) {
    cpu->push(cpu->A);
}

// PHP - Push Processor Status
void CPU::php(CPU * cpu, stepInfo * info) {
    cpu->push(cpu->flags() | 0x10);
}

// PLA - Pull Accumulator
void CPU::pla(CPU * cpu, stepInfo * info) {
    cpu->A = cpu->pull();
    cpu->setZN(cpu->A);
}

// PLP - Pull Processor Status
void CPU::plp(CPU * cpu, stepInfo * info) {
    cpu->setFlags((cpu->pull()&0xEF) | 0x20);
}

// ROL - Rotate Left
void CPU::rol(CPU * cpu, stepInfo * info) {
    if (info->mode == modeAccumulator) {
        Byte c = cpu->C;
        cpu->C = (cpu->A >> 7) & 1;
        cpu->A = (cpu->A << 1) | c;
        cpu->setZN(cpu->A);
    } else {
        Byte c = cpu->C;
        Byte value = cpu->memory.read(info->address);
        cpu->C = (value >> 7) & 1;
        value = (value << 1) | c;
        cpu->memory.write(info->address, value);
        cpu->setZN(value);
    }
}

// ROR - Rotate Right
void CPU::ror(CPU * cpu, stepInfo * info)
{
    if (info->mode == modeAccumulator) {
        Byte c = cpu->C;
        cpu->C = cpu->A & 1;
        cpu->A = (cpu->A >> 1) | (c << 7);
        cpu->setZN(cpu->A);
    } else {
        Byte c = cpu->C;
        Byte value = cpu->memory.read(info->address);
        cpu->C = value & 1;
        value = (value >> 1) | (c << 7);
        cpu->memory.write(info->address, value);
        cpu->setZN(value);
    }
}

// RTI - Return from Interrupt
void CPU::rti(CPU * cpu, stepInfo * info)
{
    cpu->setFlags((cpu->pull()&0xEF) | 0x20);
    cpu->PC = cpu->pull16();
}

// RTS - Return from Subroutine
void CPU::rts(CPU * cpu, stepInfo * info)
{
    cpu->PC = cpu->pull16() + 1;
}

// SBC - Subtract with Carry
void CPU::sbc(CPU * cpu, stepInfo * info)
{
    Byte a = cpu->A;
    Byte b = cpu->memory.read(info->address);
    Byte c = cpu->C;
    cpu->A = a - b - (1 - c);
    cpu->setZN(cpu->A);
    
    int carry = ((int) a) - ((int) b) - ((int) 1-c);
    
    
    if (carry >= 0) {
        cpu->C = 1;
    } else {
        cpu->C = 0;
    }
    if (((a^b)&0x80) != 0 && ((a^cpu->A)&0x80) != 0) {
        cpu->V = 1;
    } else {
        cpu->V = 0;
    }
}


void CPU::sec(CPU * cpu, stepInfo * info)
{
    cpu->C = 1;
}

// SED - Set Decimal Flag
void CPU::sed(CPU * cpu, stepInfo * info)
{
    cpu->D = 1;
}

// SEI - Set Interrupt Disable
void CPU::sei(CPU * cpu, stepInfo * info)
{
    cpu->I = 1;
}

// STA - Store Accumulator
void CPU::sta(CPU * cpu, stepInfo * info)
{
    cpu->memory.write(info->address, cpu->A);
}

// STX - Store X Register
void CPU::stx(CPU * cpu, stepInfo * info)
{
    cpu->memory.write(info->address, cpu->X);
}

// STY - Store Y Register
void CPU::sty(CPU * cpu, stepInfo * info)
{
    cpu->memory.write(info->address, cpu->Y);
}

// TAX - Transfer Accumulator to X
void CPU::tax(CPU * cpu, stepInfo * info)
{
    cpu->X = cpu->A;
    cpu->setZN(cpu->X);
}

// TAY - Transfer Accumulator to Y
void CPU::tay(CPU * cpu, stepInfo * info)
{
    cpu->Y = cpu->A;
    cpu->setZN(cpu->Y);
}

// TSX - Transfer Stack Pointer to X
void CPU::tsx(CPU * cpu, stepInfo * info)
{
    cpu->X = cpu->SP;
    cpu->setZN(cpu->X);
}

// TXA - Transfer X to Accumulator
void CPU::txa(CPU * cpu, stepInfo * info)
{
    cpu->A = cpu->X;
    cpu->setZN(cpu->A);
}

// TXS - Transfer X to Stack Pointer
void CPU::txs(CPU * cpu, stepInfo * info)
{
    cpu->SP = cpu->X;
}

// TYA - Transfer Y to Accumulator
void CPU::tya(CPU * cpu, stepInfo * info)
{
    cpu->A = cpu->Y;
    cpu->setZN(cpu->A);
}
