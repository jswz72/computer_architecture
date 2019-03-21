/*
 * MIPS pipeline timing simulator
 *
 */

#include "pipe.h"
#include "shell.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>



/* global pipeline state */
CPU_State CURRENT_STATE;

// Pipeline registers
Pipe_Reg_IFDE PIPE_REG_IFDE = { .nop = 1 };
Pipe_Reg_IDEX PIPE_REG_IDEX = { .nop = 1 };
Pipe_Reg_EXMEM PIPE_REG_EXMEM = { .nop = 1 };
Pipe_Reg_MEMWB PIPE_REG_MEMWB = { .nop = 1 };

void pipe_init()
{
    memset(&CURRENT_STATE, 0, sizeof(CPU_State));
    CURRENT_STATE.PC = 0x00400000;
}

void pipe_cycle()
{
	pipe_stage_wb();
	pipe_stage_mem();
	pipe_stage_execute();
	pipe_stage_decode();
	pipe_stage_fetch();
}



void pipe_stage_wb()
{
    if (PIPE_REG_MEMWB.nop)
        return;
    printf("WB Executing\n");
    if (PIPE_REG_MEMWB.reg_write) {
        printf("Writing to register %d\n", PIPE_REG_MEMWB.dest);
        CURRENT_STATE.REGS[PIPE_REG_MEMWB.dest] = PIPE_REG_MEMWB.data;
        PIPE_REG_MEMWB.reg_wrote = PIPE_REG_MEMWB.dest;
        PIPE_REG_MEMWB.wrote_val = PIPE_REG_MEMWB.data;
    }
    if (PIPE_REG_MEMWB.done)
        RUN_BIT = 0;
}

void pipe_stage_mem()
{
    if (PIPE_REG_EXMEM.nop)
        return;
    PIPE_REG_MEMWB.done = PIPE_REG_EXMEM.done;
    if (PIPE_REG_EXMEM.done)
        return;
    printf("MEM Executing\n");
    PIPE_REG_MEMWB.reg_write = PIPE_REG_EXMEM.reg_write;
    switch (PIPE_REG_EXMEM.opcode) {
        // LW
        case 35:
            PIPE_REG_MEMWB.data = mem_read_32(PIPE_REG_EXMEM.ALUresult);
            printf("DEBUG2\n\n");
            printf("data: %d\n", PIPE_REG_MEMWB.data);
            printf("reg_write: %d\n", PIPE_REG_MEMWB.reg_write);
            PIPE_REG_MEMWB.dest = PIPE_REG_EXMEM.dest;
            break;
        // SW
        case 43:
            mem_write_32(PIPE_REG_EXMEM.ALUresult, PIPE_REG_EXMEM.mem_write_val);
            break;
        default:
            // For other instructions, fwd data and dest to be written to reg
            PIPE_REG_MEMWB.data = PIPE_REG_EXMEM.ALUresult;
            PIPE_REG_MEMWB.dest = PIPE_REG_EXMEM.dest;
    }
    // Update PC
    //CURRENT_STATE.PC = PIPE_REG_EXMEM.PCval;
    PIPE_REG_MEMWB.nop = 0;
}

/*************** R-TYPE EXEC FUNCS ****************/
void add()
{
    PIPE_REG_EXMEM.ALUresult = PIPE_REG_IDEX.reg_val1 + PIPE_REG_IDEX.reg_val2;
}

void addu()
{
    // overflow TODO?
    PIPE_REG_EXMEM.ALUresult = (uint32_t) (PIPE_REG_IDEX.reg_val1 + PIPE_REG_IDEX.reg_val2);
}

void sub()
{
    PIPE_REG_EXMEM.ALUresult = PIPE_REG_IDEX.reg_val1 - PIPE_REG_IDEX.reg_val2;
}

void subu()
{
    // overflow TODO?
    PIPE_REG_EXMEM.ALUresult = PIPE_REG_IDEX.reg_val1 - PIPE_REG_IDEX.reg_val2;
}

void slt()
{
	PIPE_REG_EXMEM.ALUresult = 
        PIPE_REG_IDEX.reg_val1 < PIPE_REG_IDEX.reg_val2;
}

void sltu()
{
    // overflow TODO ?
    PIPE_REG_EXMEM.ALUresult = 
        PIPE_REG_IDEX.reg_val1 < PIPE_REG_IDEX.reg_val2;

}

void execute_r() {
    switch(PIPE_REG_IDEX.ALUfunc) {
        case 32:
            add();
            break;
        case 33:
            addu();
            break;
        case 34:
            sub();
            break;
        case 35:
            subu();
            break;
        case 42:
            slt();
            break;
        case 43:
            sltu();
            break;
    }
}

/*************** I-TYPE EXEC FUNCS ****************/
void beq()
{
	int offset = 0;
	if (PIPE_REG_IDEX.reg_val1 == PIPE_REG_IDEX.reg_val2)
		offset = PIPE_REG_IDEX.imm * 4;
    PIPE_REG_EXMEM.PCval += offset;
}

void bne()
{
	int offset = 0;
	if (PIPE_REG_IDEX.reg_val1 != PIPE_REG_IDEX.reg_val2)
		offset = PIPE_REG_IDEX.imm * 4;
    PIPE_REG_EXMEM.PCval += offset;
}

void bgtz()
{
	int offset = 0;
	if (PIPE_REG_IDEX.reg_val1 > 0)
		offset = PIPE_REG_IDEX.imm * 4;
    PIPE_REG_EXMEM.PCval += offset;
}

void addi()
{
    PIPE_REG_EXMEM.ALUresult = PIPE_REG_IDEX.reg_val1 + PIPE_REG_IDEX.imm;
}

void addiu()
{
    // overflow TODO?
    PIPE_REG_EXMEM.ALUresult = PIPE_REG_IDEX.reg_val1 + (uint32_t) PIPE_REG_IDEX.imm;
}

void slti()
{
    PIPE_REG_EXMEM.ALUresult = PIPE_REG_IDEX.reg_val1 < PIPE_REG_IDEX.imm;
}

void ori()
{
    // 0-extend immediate value
    PIPE_REG_EXMEM.ALUresult = 
        PIPE_REG_IDEX.reg_val1 | (0X0000FFFF & PIPE_REG_IDEX.imm);
}

// TODO
void lui()
{
    // load imm into upper 16 bits
    PIPE_REG_EXMEM.ALUresult = PIPE_REG_IDEX.imm << 16;
}

// TODO
void lw()
{
    PIPE_REG_EXMEM.ALUresult = PIPE_REG_IDEX.reg_val1 + PIPE_REG_IDEX.imm;
}

// TODO
void sw()
{
    PIPE_REG_EXMEM.ALUresult = PIPE_REG_IDEX.reg_val1 + PIPE_REG_IDEX.imm;
    PIPE_REG_EXMEM.mem_write_val = PIPE_REG_IDEX.reg_val2;
}

void execute_i()
{
	switch(PIPE_REG_IDEX.opcode) {
		case 4:
			beq();
			break;
		case 5:
			bne();
			break;
		case 7:
			bgtz();
			break;
		case 8:
			addi();
			break;
		case 9:
			addiu();
			break;
		case 10:
			slti();
			break;
		case 13:
			ori();
			break;
		case 15:
			lui();
			break;
		case 35:
			lw();
			break;
		case 43:
			sw();
			break;
	}
    // Pass pcval through if doesn't need to be modified
	if (PIPE_REG_IDEX.opcode > 7)
        PIPE_REG_EXMEM.PCval = PIPE_REG_IDEX.PCval;
}



void pipe_stage_execute()
{
    if (PIPE_REG_IDEX.nop)
        return;
    PIPE_REG_EXMEM.done = PIPE_REG_IDEX.done;
    if (PIPE_REG_IDEX.done)
        return;
    printf("Ex Executing\n");

    
    // Forwarding
    int exhazard1 = 0;
    int exhazard2 = 0;
    if (PIPE_REG_IDEX.opcode == 43) {
        printf("DEBUG\n\n");
        printf("IDEXrt: %d, IDEXrs %d\n", PIPE_REG_IDEX.rt, PIPE_REG_IDEX.rs);
        printf("rtval: %d, rsval %d\n", PIPE_REG_IDEX.reg_val2, PIPE_REG_IDEX.reg_val1);
        printf("\n");
    }
    if (PIPE_REG_EXMEM.reg_write && PIPE_REG_EXMEM.dest) {
        if (PIPE_REG_EXMEM.dest == PIPE_REG_IDEX.rs) {
            printf("Yesx rs %d, %d\n", PIPE_REG_EXMEM.dest, PIPE_REG_EXMEM.ALUresult);
            PIPE_REG_IDEX.reg_val1 = PIPE_REG_EXMEM.ALUresult;
            exhazard1 = 1;
        }
        if (PIPE_REG_EXMEM.dest == PIPE_REG_IDEX.rt) {
            printf("Yesx rt %d, %d\n", PIPE_REG_EXMEM.dest, PIPE_REG_EXMEM.ALUresult);
            PIPE_REG_IDEX.reg_val2 = PIPE_REG_EXMEM.ALUresult;
            exhazard2 = 1;
        }
    }
    if (PIPE_REG_MEMWB.reg_wrote) {
        // Don't fwd if exhazard already fwded
        if (PIPE_REG_MEMWB.reg_wrote == PIPE_REG_IDEX.rs && !exhazard1) {
            printf("Yesw rs %d, %d\n", PIPE_REG_MEMWB.reg_wrote, PIPE_REG_MEMWB.wrote_val);
            PIPE_REG_IDEX.reg_val1 = PIPE_REG_MEMWB.wrote_val;
        }
        if (PIPE_REG_MEMWB.reg_wrote == PIPE_REG_IDEX.rt && !exhazard2) {
            printf("Yesw rt %d, %d\n", PIPE_REG_MEMWB.reg_wrote, PIPE_REG_MEMWB.wrote_val);
            PIPE_REG_IDEX.reg_val2 = PIPE_REG_MEMWB.wrote_val;
        }
    }


    uint32_t opcode = PIPE_REG_IDEX.opcode;
    if (opcode == 0) {
        execute_r();
        PIPE_REG_EXMEM.PCval = PIPE_REG_IDEX.PCval;
    }
    else if (opcode == 2)
        // J instruction
        PIPE_REG_EXMEM.PCval = PIPE_REG_IDEX.addr;
    else if (opcode > 3)
        execute_i();
    PIPE_REG_EXMEM.opcode = opcode;
    // Will write reg if not load or store (with current instruction set of project)
    PIPE_REG_EXMEM.reg_write = opcode != 43; // TODO
    PIPE_REG_EXMEM.dest = PIPE_REG_IDEX.dest;
    PIPE_REG_EXMEM.nop = 0;
}

void decode_r(uint32_t instruction)
{
    // Use masks to get parts of instruction, shift for next part
	PIPE_REG_IDEX.ALUfunc = instruction & 0x3F;
	instruction >>= 11;
	PIPE_REG_IDEX.dest = instruction & 0x1F;
	instruction >>= 5;
    // rt
    PIPE_REG_IDEX.rt = instruction & 0x1F;
	PIPE_REG_IDEX.reg_val2 = CURRENT_STATE.REGS[PIPE_REG_IDEX.rt];
	instruction >>= 5;
    // rs
    PIPE_REG_IDEX.rs = instruction & 0x1F;
	PIPE_REG_IDEX.reg_val1 = CURRENT_STATE.REGS[PIPE_REG_IDEX.rs];
}

void decode_j(uint32_t instruction)
{
    // Get 26 bits for instruction
	uint32_t addr = instruction & 0X3FFFFFF;
    // Final instruction shifted by 2, with 4 high bites from next pc
    addr <<= 2;
    PIPE_REG_IDEX.addr = (PIPE_REG_IFDE.PCval & 0XF0000000) | addr;
}

int32_t sign_extend(int16_t a)
{
    // Extend the 16th bit
    int mask = 0x8000;
    if (mask & a) {
        a = a | 0xFFFF0000;
    }
    return a;
}

void decode_i(uint32_t instruction)
{
    PIPE_REG_IDEX.imm = sign_extend(instruction & 0xFFFF);
	instruction >>= 16;
	PIPE_REG_IDEX.rt = instruction & 0x1F;
	PIPE_REG_IDEX.dest = PIPE_REG_IDEX.rt;
	PIPE_REG_IDEX.reg_val2 = CURRENT_STATE.REGS[PIPE_REG_IDEX.rt];
	instruction >>= 5;
    PIPE_REG_IDEX.rs = instruction & 0x1F;
	PIPE_REG_IDEX.reg_val1 = CURRENT_STATE.REGS[PIPE_REG_IDEX.rs];
	instruction >>= 5;
}

void pipe_stage_decode()
{
    if (PIPE_REG_IFDE.nop)
        return;
    uint32_t instruction = PIPE_REG_IFDE.instruction;
    // Tell next stage no instructions left
    PIPE_REG_IDEX.done = !instruction || PIPE_REG_IDEX.done;
    if (PIPE_REG_IDEX.done)
        return;
    printf("Decode executing\n");
    uint32_t opcode = instruction >> 26;
    // Pass on PCval and opcode
    PIPE_REG_IDEX.opcode = opcode;
    PIPE_REG_IDEX.PCval = PIPE_REG_IFDE.PCval;

	if (opcode == 0)
		decode_r(instruction);
	else if (opcode == 2 || opcode == 3)
		decode_j(instruction);
	else if (opcode > 3)
		decode_i(instruction);
    PIPE_REG_IDEX.nop = 0;
}

void pipe_stage_fetch()
{
    printf("Fetch executing\n");
    // Read instruction from instruction memory
    PIPE_REG_IFDE.instruction = mem_read_32(CURRENT_STATE.PC);
    // Pass incremented PC value to next stages
    PIPE_REG_IFDE.PCval = CURRENT_STATE.PC + 4;
    CURRENT_STATE.PC = CURRENT_STATE.PC + 4;
    PIPE_REG_IFDE.nop = 0;
}
