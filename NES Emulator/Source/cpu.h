//
//  cpu.h
//  NES_Emulator_Go_Port
//
//  Created by Samuel Hunt on 19/02/2018.
//
//

#ifndef __NES_Emulator_Go_Port__cpu__
#define __NES_Emulator_Go_Port__cpu__

#include "../JuceLibraryCode/JuceHeader.h"
#include "memory.h"

static const int CPUFrequency = 1789773;

// interrupt types
enum eIota {
       interruptNone = 0,
    interruptNMI,
    interruptIRQ
};
// addressing modes
enum eAddressingModes {
    modeAbsolute = 1,
    modeAbsoluteX,
    modeAbsoluteY,
    modeAccumulator,
    modeImmediate,
    modeImplied,
    modeIndexedIndirect,
    modeIndirect,
    modeIndirectIndexed,
    modeRelative,
    modeZeroPage,
    modeZeroPageX,
    modeZeroPageY,
};

// stepInfo contains information that the instruction functions use
struct stepInfo {
	UInt16  address;
	UInt16  pc;
	Byte    mode;
};


// instructionModes indicates the addressing mode for each instruction
static Byte instructionModes[256] = {
	6, 7, 6, 7, 11, 11, 11, 11, 6, 5, 4, 5, 1, 1, 1, 1,
	10, 9, 6, 9, 12, 12, 12, 12, 6, 3, 6, 3, 2, 2, 2, 2,
	1, 7, 6, 7, 11, 11, 11, 11, 6, 5, 4, 5, 1, 1, 1, 1,
	10, 9, 6, 9, 12, 12, 12, 12, 6, 3, 6, 3, 2, 2, 2, 2,
	6, 7, 6, 7, 11, 11, 11, 11, 6, 5, 4, 5, 1, 1, 1, 1,
	10, 9, 6, 9, 12, 12, 12, 12, 6, 3, 6, 3, 2, 2, 2, 2,
	6, 7, 6, 7, 11, 11, 11, 11, 6, 5, 4, 5, 8, 1, 1, 1,
	10, 9, 6, 9, 12, 12, 12, 12, 6, 3, 6, 3, 2, 2, 2, 2,
	5, 7, 5, 7, 11, 11, 11, 11, 6, 5, 6, 5, 1, 1, 1, 1,
	10, 9, 6, 9, 12, 12, 13, 13, 6, 3, 6, 3, 2, 2, 3, 3,
	5, 7, 5, 7, 11, 11, 11, 11, 6, 5, 6, 5, 1, 1, 1, 1,
	10, 9, 6, 9, 12, 12, 13, 13, 6, 3, 6, 3, 2, 2, 3, 3,
	5, 7, 5, 7, 11, 11, 11, 11, 6, 5, 6, 5, 1, 1, 1, 1,
	10, 9, 6, 9, 12, 12, 12, 12, 6, 3, 6, 3, 2, 2, 2, 2,
	5, 7, 5, 7, 11, 11, 11, 11, 6, 5, 6, 5, 1, 1, 1, 1,
	10, 9, 6, 9, 12, 12, 12, 12, 6, 3, 6, 3, 2, 2, 2, 2,
};

// instructionSizes indicates the size of each instruction in bytes
static Byte instructionSizes[256] = {
	1, 2, 0, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0,
	2, 2, 0, 0, 2, 2, 2, 0, 1, 3, 1, 0, 3, 3, 3, 0,
	3, 2, 0, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0,
	2, 2, 0, 0, 2, 2, 2, 0, 1, 3, 1, 0, 3, 3, 3, 0,
	1, 2, 0, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0,
	2, 2, 0, 0, 2, 2, 2, 0, 1, 3, 1, 0, 3, 3, 3, 0,
	1, 2, 0, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0,
	2, 2, 0, 0, 2, 2, 2, 0, 1, 3, 1, 0, 3, 3, 3, 0,
	2, 2, 0, 0, 2, 2, 2, 0, 1, 0, 1, 0, 3, 3, 3, 0,
	2, 2, 0, 0, 2, 2, 2, 0, 1, 3, 1, 0, 0, 3, 0, 0,
	2, 2, 2, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0,
	2, 2, 0, 0, 2, 2, 2, 0, 1, 3, 1, 0, 3, 3, 3, 0,
	2, 2, 0, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0,
	2, 2, 0, 0, 2, 2, 2, 0, 1, 3, 1, 0, 3, 3, 3, 0,
	2, 2, 0, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0,
	2, 2, 0, 0, 2, 2, 2, 0, 1, 3, 1, 0, 3, 3, 3, 0,
};

// instructionCycles indicates the number of cycles used by each instruction,
// not including conditional cycles
static Byte instructionCycles[256] {
	7, 6, 2, 8, 3, 3, 5, 5, 3, 2, 2, 2, 4, 4, 6, 6,
	2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
	6, 6, 2, 8, 3, 3, 5, 5, 4, 2, 2, 2, 4, 4, 6, 6,
	2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
	6, 6, 2, 8, 3, 3, 5, 5, 3, 2, 2, 2, 3, 4, 6, 6,
	2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
	6, 6, 2, 8, 3, 3, 5, 5, 4, 2, 2, 2, 5, 4, 6, 6,
	2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
	2, 6, 2, 6, 3, 3, 3, 3, 2, 2, 2, 2, 4, 4, 4, 4,
	2, 6, 2, 6, 4, 4, 4, 4, 2, 5, 2, 5, 5, 5, 5, 5,
	2, 6, 2, 6, 3, 3, 3, 3, 2, 2, 2, 2, 4, 4, 4, 4,
	2, 5, 2, 5, 4, 4, 4, 4, 2, 4, 2, 4, 4, 4, 4, 4,
	2, 6, 2, 8, 3, 3, 5, 5, 2, 2, 2, 2, 4, 4, 6, 6,
	2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
	2, 6, 2, 8, 3, 3, 5, 5, 2, 2, 2, 2, 4, 4, 6, 6,
	2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
};

// instructionPageCycles indicates the number of cycles used by each
// instruction when a page is crossed
static Byte instructionPageCycles[256] {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
};

// instructionNames indicates the name of each instruction
static const char * instructionNames[256] = {
	"BRK", "ORA", "KIL", "SLO", "NOP", "ORA", "ASL", "SLO",
	"PHP", "ORA", "ASL", "ANC", "NOP", "ORA", "ASL", "SLO",
	"BPL", "ORA", "KIL", "SLO", "NOP", "ORA", "ASL", "SLO",
	"CLC", "ORA", "NOP", "SLO", "NOP", "ORA", "ASL", "SLO",
	"JSR", "AND", "KIL", "RLA", "BIT", "AND", "ROL", "RLA",
	"PLP", "AND", "ROL", "ANC", "BIT", "AND", "ROL", "RLA",
	"BMI", "AND", "KIL", "RLA", "NOP", "AND", "ROL", "RLA",
	"SEC", "AND", "NOP", "RLA", "NOP", "AND", "ROL", "RLA",
	"RTI", "EOR", "KIL", "SRE", "NOP", "EOR", "LSR", "SRE",
	"PHA", "EOR", "LSR", "ALR", "JMP", "EOR", "LSR", "SRE",
	"BVC", "EOR", "KIL", "SRE", "NOP", "EOR", "LSR", "SRE",
	"CLI", "EOR", "NOP", "SRE", "NOP", "EOR", "LSR", "SRE",
	"RTS", "ADC", "KIL", "RRA", "NOP", "ADC", "ROR", "RRA",
	"PLA", "ADC", "ROR", "ARR", "JMP", "ADC", "ROR", "RRA",
	"BVS", "ADC", "KIL", "RRA", "NOP", "ADC", "ROR", "RRA",
	"SEI", "ADC", "NOP", "RRA", "NOP", "ADC", "ROR", "RRA",
	"NOP", "STA", "NOP", "SAX", "STY", "STA", "STX", "SAX",
	"DEY", "NOP", "TXA", "XAA", "STY", "STA", "STX", "SAX",
	"BCC", "STA", "KIL", "AHX", "STY", "STA", "STX", "SAX",
	"TYA", "STA", "TXS", "TAS", "SHY", "STA", "SHX", "AHX",
	"LDY", "LDA", "LDX", "LAX", "LDY", "LDA", "LDX", "LAX",
	"TAY", "LDA", "TAX", "LAX", "LDY", "LDA", "LDX", "LAX",
	"BCS", "LDA", "KIL", "LAX", "LDY", "LDA", "LDX", "LAX",
	"CLV", "LDA", "TSX", "LAS", "LDY", "LDA", "LDX", "LAX",
	"CPY", "CMP", "NOP", "DCP", "CPY", "CMP", "DEC", "DCP",
	"INY", "CMP", "DEX", "AXS", "CPY", "CMP", "DEC", "DCP",
	"BNE", "CMP", "KIL", "DCP", "NOP", "CMP", "DEC", "DCP",
	"CLD", "CMP", "NOP", "DCP", "NOP", "CMP", "DEC", "DCP",
	"CPX", "SBC", "NOP", "ISC", "CPX", "SBC", "INC", "ISC",
	"INX", "SBC", "NOP", "SBC", "CPX", "SBC", "INC", "ISC",
	"BEQ", "SBC", "KIL", "ISC", "NOP", "SBC", "INC", "ISC",
	"SED", "SBC", "NOP", "ISC", "NOP", "SBC", "INC", "ISC",
};






class Console;
class CPU {
public:
    
    typedef void (*InstructionMap) (CPU*, stepInfo*);
    
    CPU (Console * console);
    void init ();
    
    // Reset resets the CPU to its initial powerup state
    void Reset();
    
    
    static bool pagesDiffer(UInt16 a, UInt16 b);
    
    // addBranchCycles adds a cycle for taking a branch and adds another cycle
    // if the branch jumps to a new page
    void addBranchCycles(stepInfo info);


    void compare(Byte a, Byte b);
    
    // Read16 reads two bytes using Read to return a double-word value
    
    UInt16 read16(UInt16 address);
    
    // read16bug emulates a 6502 bug that caused the low byte to wrap without
    // incrementing the high byte
    UInt16 read16Bug(UInt16 address);
 
    void push (Byte value);
    Byte pull ();
    
    // push16 pushes two bytes onto the stack
    void push16 (UInt16 value);
    UInt16 pull16 ();
    
    // Flags returns the processor status flags
    Byte flags ();
        // SetFlags sets the processor status flags
    void setFlags (Byte flags);
    
    
    
    // setZ sets the zero flag if the argument is zero
    void setZ (Byte value);
    
    // setN sets the negative flag if the argument is negative (high bit is set)
    void setN (Byte value);
    
    // setZN sets the zero flag and the negative flag
    void setZN (Byte value);
    
    // triggerNMI causes a non-maskable interrupt to occur on the next cycle
    void triggerNMI ();
    
    // triggerIRQ causes an IRQ interrupt to occur on the next cycle
    void triggerIRQ ();
    
    // Step executes a single CPU instruction
    int step ();

    // PrintInstruction prints the current CPU state
    void printInstruction ();
    
    
    // NMI - Non-Maskable Interrupt
    void nmi ();
    // IRQ - IRQ Interrupt
    void irq ();
    
    

    // createTable builds a function table for each instruction
    InstructionMap map[256] = {
        brk, ora, kil, slo, nop, ora, asl, slo,
		php, ora, asl, anc, nop, ora, asl, slo,
		bpl, ora, kil, slo, nop, ora, asl, slo,
		clc, ora, nop, slo, nop, ora, asl, slo,
		jsr, And, kil, rla, bit, And, rol, rla,
		plp, And, rol, anc, bit, And, rol, rla,
		bmi, And, kil, rla, nop, And, rol, rla,
		sec, And, nop, rla, nop, And, rol, rla,
		rti, eor, kil, sre, nop, eor, lsr, sre,
		pha, eor, lsr, alr, jmp, eor, lsr, sre,
		bvc, eor, kil, sre, nop, eor, lsr, sre,
		cli, eor, nop, sre, nop, eor, lsr, sre,
		rts, adc, kil, rra, nop, adc, ror, rra,
		pla, adc, ror, arr, jmp, adc, ror, rra,
		bvs, adc, kil, rra, nop, adc, ror, rra,
		sei, adc, nop, rra, nop, adc, ror, rra,
		nop, sta, nop, sax, sty, sta, stx, sax,
		dey, nop, txa, xaa, sty, sta, stx, sax,
		bcc, sta, kil, ahx, sty, sta, stx, sax,
		tya, sta, txs, tas, shy, sta, shx, ahx,
		ldy, lda, ldx, lax, ldy, lda, ldx, lax,
		tay, lda, tax, lax, ldy, lda, ldx, lax,
		bcs, lda, kil, lax, ldy, lda, ldx, lax,
		clv, lda, tsx, las, ldy, lda, ldx, lax,
		cpy, cmp, nop, dcp, cpy, cmp, dec, dcp,
		iny, cmp, dex, axs, cpy, cmp, dec, dcp,
		bne, cmp, kil, dcp, nop, cmp, dec, dcp,
		cld, cmp, nop, dcp, nop, cmp, dec, dcp,
		cpx, sbc, nop, isc, cpx, sbc, inc, isc,
		inx, sbc, nop, sbc, cpx, sbc, inc, isc,
		beq, sbc, kil, isc, nop, sbc, inc, isc,
		sed, sbc, nop, isc, nop, sbc, inc, isc,
    };
    
    
    
    //add with carry
    static void adc(CPU * cpu, stepInfo * info);
    
    // AND - Logical AND
    static void And(CPU * cpu, stepInfo * info);
    
    // ASL - Arithmetic Shift Left
    static void asl(CPU * cpu, stepInfo * info);
    
    
    
    
    // BCC - Branch if Carry Clear
    static void bcc(CPU * cpu, stepInfo * info);
    
    // BCS - Branch if Carry Set
    static void bcs(CPU * cpu, stepInfo * info);
    
    // BEQ - Branch if Equal
    static void beq(CPU * cpu, stepInfo * info);

    // BIT - Bit Test
    static void bit(CPU * cpu, stepInfo * info);
    
    // BMI - Branch if Minus
    static void bmi(CPU * cpu, stepInfo * info);

    // BNE - Branch if Not Equal
    static void bne(CPU * cpu, stepInfo * info);
    
    // BPL - Branch if Positive
    static void bpl(CPU * cpu, stepInfo * info);
    
    
    // BRK - Force Interrupt
    static void brk(CPU * cpu, stepInfo * info);
    
    // BVC - Branch if Overflow Clear
    static void bvc(CPU * cpu, stepInfo * info);
    
    // BVS - Branch if Overflow Set
    static void bvs(CPU * cpu, stepInfo * info);
    
    
    
    
    // CLC - Clear Carry Flag
    static void clc(CPU * cpu, stepInfo * info);
    // CLD - Clear Decimal Mode
    static void cld(CPU * cpu, stepInfo * info);

    
    // CLI - Clear Interrupt Disable
    static void cli(CPU * cpu, stepInfo * info);
    
    // CLV - Clear Overflow Flag
    static void clv(CPU * cpu, stepInfo * info);

    
    
    // CMP - Compare
        static void cmp(CPU * cpu, stepInfo * info);
    
    // CPX - Compare X Register
        static void cpx(CPU * cpu, stepInfo * info);
    // CPY - Compare Y Register
        static void cpy(CPU * cpu, stepInfo * info);
    
    // DEC - Decrement Memory
    static void dec(CPU * cpu, stepInfo * info);
    // DEX - Decrement X Register
    static void dex(CPU * cpu, stepInfo * info);
    // DEY - Decrement Y Register
    static void dey(CPU * cpu, stepInfo * info);
    // EOR - Exclusive OR
    static void eor(CPU * cpu, stepInfo * info);
    // INC - Increment Memory
    static void inc(CPU * cpu, stepInfo * info);
    // INX - Increment X Register
    static void inx(CPU * cpu, stepInfo * info);
    // INY - Increment Y Register
    static void iny(CPU * cpu, stepInfo * info);
    // JMP - Jump
    static void jmp(CPU * cpu, stepInfo * info);
    // JSR - Jump to Subroutine
    static void jsr(CPU * cpu, stepInfo * info);
    
    // LDA - Load Accumulator
    static void lda(CPU * cpu, stepInfo * info);
    // LDX - Load X Register
    static void ldx(CPU * cpu, stepInfo * info);
    
    // LDY - Load Y Register
    static void ldy(CPU * cpu, stepInfo * info);
    

    
    
    
    
    // LSR - Logical Shift Right
    static void lsr(CPU * cpu, stepInfo * info);
    
    // NOP - No Operation
    static void nop(CPU * cpu, stepInfo * info);
    
    // ORA - Logical Inclusive OR
    static void ora(CPU * cpu, stepInfo * info);
    
    // PHA - Push Accumulator
    static void pha(CPU * cpu, stepInfo * info);
    
    // PHP - Push Processor Status
    static void php(CPU * cpu, stepInfo * info);
    
    // PLA - Pull Accumulator
    static void pla(CPU * cpu, stepInfo * info);
    
    // PLP - Pull Processor Status
    static void plp(CPU * cpu, stepInfo * info);
    
    // ROL - Rotate Left
    static void rol(CPU * cpu, stepInfo * info);
    
    // ROR - Rotate Right
    static void ror(CPU * cpu, stepInfo * info);
    
    // RTI - Return from Interrupt
    static void rti(CPU * cpu, stepInfo * info);
    
    // RTS - Return from Subroutine
    static void rts(CPU * cpu, stepInfo * info);
    
    // SBC - Subtract with Carry
    static void sbc(CPU * cpu, stepInfo * info); 
    
    
    // SEC - Set Carry Flag
    static void sec(CPU * cpu, stepInfo * info);
    
    // SED - Set Decimal Flag
    static void sed(CPU * cpu, stepInfo * info);
    
    // SEI - Set Interrupt Disable
    static void sei(CPU * cpu, stepInfo * info);
    
    // STA - Store Accumulator
    static void sta(CPU * cpu, stepInfo * info);
    
    // STX - Store X Register
    static void stx(CPU * cpu, stepInfo * info);
    
    // STY - Store Y Register
    static void sty(CPU * cpu, stepInfo * info);
    
    // TAX - Transfer Accumulator to X
    static void tax(CPU * cpu, stepInfo * info);
    
    // TAY - Transfer Accumulator to Y
    static void tay(CPU * cpu, stepInfo * info);

    // TSX - Transfer Stack Pointer to X
    static void tsx(CPU * cpu, stepInfo * info);
    
    // TXA - Transfer X to Accumulator
    static void txa(CPU * cpu, stepInfo * info);
    
    // TXS - Transfer X to Stack Pointer
    static void txs(CPU * cpu, stepInfo * info);
    
    // TYA - Transfer Y to Accumulator
    static void tya(CPU * cpu, stepInfo * info); 
    
    
    
    
    // illegal opcodes below
    
    static void ahx(CPU * cpu, stepInfo * info) {jassertfalse;}
    static void alr(CPU * cpu, stepInfo * info) {jassertfalse;}
    static void anc(CPU * cpu, stepInfo * info) {jassertfalse;}
    static void arr(CPU * cpu, stepInfo * info) {jassertfalse;}
    static void axs(CPU * cpu, stepInfo * info) {jassertfalse;}
    static void dcp(CPU * cpu, stepInfo * info) {jassertfalse;}
    static void isc(CPU * cpu, stepInfo * info) {jassertfalse;}
    static void kil(CPU * cpu, stepInfo * info) {jassertfalse;}
    static void las(CPU * cpu, stepInfo * info) {jassertfalse;}
    static void lax(CPU * cpu, stepInfo * info) {jassertfalse;}
    static void rla(CPU * cpu, stepInfo * info) {jassertfalse;}
    static void rra(CPU * cpu, stepInfo * info) {//jassertfalse;
    }
    static void sax(CPU * cpu, stepInfo * info) {jassertfalse;}
    static void shx(CPU * cpu, stepInfo * info) {jassertfalse;}
    static void shy(CPU * cpu, stepInfo * info) {jassertfalse;}
    static void slo(CPU * cpu, stepInfo * info) {jassertfalse;}
    static void sre(CPU * cpu, stepInfo * info) {jassertfalse;}
    static void tas(CPU * cpu, stepInfo * info) {jassertfalse;}
    static void xaa(CPU * cpu, stepInfo * info) {jassertfalse;}

    cpuMemory  memory;

	uint64  Cycles; // number of cycles
	uint16  PC; // program counter
	Byte    SP;   // stack pointer
	Byte    A;   // accumulator
	Byte    X;   // x register
	Byte    Y;   // y register
	Byte    C;   // carry flag
	Byte    Z;   // zero flag
	Byte    I;  // interrupt disable flag
	Byte    D;   // decimal mode flag
	Byte    B;   // break command flag
	Byte    U;   // unused flag
	Byte    V;   // overflow flag
	Byte    N;   // negative flag
	Byte    interrupt;   // interrupt type to perform
	int     stall;// number of cycles to stall
//	table     [256]func(*stepInfo)
    
};

    
#if 0
    
    static void Save(encoder *gob.Encoder) error {
        encoder.Encode(cpu.Cycles)
        encoder.Encode(cpu.PC)
        encoder.Encode(cpu.SP)
        encoder.Encode(cpu.A)
        encoder.Encode(cpu.X)
        encoder.Encode(cpu.Y)
        encoder.Encode(cpu.C)
        encoder.Encode(cpu.Z)
        encoder.Encode(cpu.I)
        encoder.Encode(cpu.D)
        encoder.Encode(cpu.B)
        encoder.Encode(cpu.U)
        encoder.Encode(cpu.V)
        encoder.Encode(cpu.N)
        encoder.Encode(cpu.interrupt)
        encoder.Encode(cpu.stall)
        return nil
    }
    
    static void Load(decoder *gob.Decoder) error {
        decoder.Decode(&cpu.Cycles)
        decoder.Decode(&cpu.PC)
        decoder.Decode(&cpu.SP)
        decoder.Decode(&cpu.A)
        decoder.Decode(&cpu.X)
        decoder.Decode(&cpu.Y)
        decoder.Decode(&cpu.C)
        decoder.Decode(&cpu.Z)
        decoder.Decode(&cpu.I)
        decoder.Decode(&cpu.D)
        decoder.Decode(&cpu.B)
        decoder.Decode(&cpu.U)
        decoder.Decode(&cpu.V)
        decoder.Decode(&cpu.N)
        decoder.Decode(&cpu.interrupt)
        decoder.Decode(&cpu.stall)
        return nil
    }
    
#endif
    
    
#endif /* defined(__NES_Emulator_Go_Port__cpu__) */



