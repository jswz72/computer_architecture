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
Pipe_Reg_IFDE PIPE_REG_IFDE;
Pipe_Reg_IFDE PIPE_REG_IDEX;
Pipe_Reg_IFDE PIPE_REG_EXMEM;

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
    // modify cur state
}

void pipe_stage_mem()
{
}

void pipe_stage_execute()
{
    uint32_t opcode = PIPE_REG_IDEX.op;
    if (opcode == 0) {
        PIPE_REG_EXMEM.dest = PIPE_REG_IDEX.rd;
        execute_r();
    }
    else if (opcode == 2 || opcode == 3)
        execute_j();
    else if (opcode > 3) {
        PIPE_REG_EXMEM.dest = PIPE_REG_IDEX.rt;
        execute_i();
    }
}

void execute_r() {
    switch(PIPE_REG_IDEX.ALUop) {
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
    inc_pc(0);
    if (!NEXT_STATE.REGS[rd])
        NEXT_STATE.FLAG_Z = 1;
}

/*************** R-TYPE EXEC FUNCS ****************/
void add()
{
    int32_t rs_val = CURRENT_STATE.REGS[PIPE_REG_IDEX.rs];
    int32_t rt_val = CURRENT_STATE.REGS[PIPE_REG_IDEX.rt];
    int32_t res = rs_val + rt_val;
    check_overflow(rs_val, rt_val, res);
    PIPE_REG_EXMEM.data = res;
}

void addu()
{
    uint32_t rs_val = CURRENT_STATE.REGS[PIPE_REG_IDEX.rs];
    uint32_t rt_val = CURRENT_STATE.REGS[PIPE_REG_IDEX.rt];
    unsigned int res = rs_val + rt_val;
	PIPE_REG_EXMEM.data = (uint32_t) res;
}

void sub()
{
    int32_t rs_val = CURRENT_STATE.REGS[PIPE_REG_IDEX.rs];
    int32_t rt_val = CURRENT_STATE.REGS[PIPE_REG_IDEX.rt];
    int32_t res = rs_val - rt_val;
    check_overflow(rs_val, rt_val, res);
    PIPE_REG_EXMEM.data = res;
}

void subu()
{
    uint32_t rs_val = CURRENT_STATE.REGS[PIPE_REG_IDEX.rs];
    uint32_t rt_val = CURRENT_STATE.REGS[PIPE_REG_IDEX.rt];
    int res = rs_val - rt_val;
	PIPE_REG_EXMEM.data = res;
}

void slt()
{
	PIPE_REG_EXMEM.data = 
        CURRENT_STATE.REGS[PIPE_REG_IDEX.rs] < CURRENT_STATE.REGS[PIPE_REG_IDEX.rt];
}

void sltu()
{
    uint32_t rs_val = CURRENT_STATE.REGS[PIPE_REG_IDEX.rs];
    uint32_t rt_val = CURRENT_STATE.REGS[PIPE_REG_IDEX.rt];
	PIPE_REG_EXMEM.data = rs_val < rt_val;
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
	if (opcode > 7)
		inc_pc(0);
}

/*************** I-TYPE EXEC FUNCS ****************/
void beq()
{
	int offset = 0;
	if (CURRENT_STATE.REGS[PIPE_REG_IDEX.rs] == CURRENT_STATE.REGS[PIPE_REG_IDEX.rt])
		offset = PIPE_REG_IDEX.imm * 4;
    PIPE_REG_EXMEM.offset = offset;
}

void bne()
{
	int offset = 0;
	if (CURRENT_STATE.REGS[PIPE_REG_IDEX.rs] != CURRENT_STATE.REGS[PIPE_REG_IDEX.rt])
		offset = PIPE_REG_IDEX.imm * 4;
    PIPE_REG_EXMEM.offset = offset;
}

void bgtz()
{
	int offset = 0;
	if (CURRENT_STATE.REGS[PIPE_REG_IDEX.rs] > 0)
		offset = PIPE_REG_IDEX.imm * 4;
    PIPE_REG_EXMEM.offset = offset;
}

void addi()
{
    int32_t rs_val = CURRENT_STATE.REGS[PIPE_REG_IDEX.rs];
    int32_t res = rs_val + PIPE_REG_IDEX.imm;
    check_overflow(rs_val, PIPE_REG_IDEX.imm, res);
    PIPE_REG_EXMEM.data = res;
}

void addiu()
{
    uint32_t rs_val = CURRENT_STATE.REGS[rs];
    int res = rs_val + (uint32_t) PIPE_REG_IDEX.imm;
    PIPE_REG_EXMEM.data = res;
}

void slti()
{
    PIPE_REG_EXMEM.data = CURRENT_STATE.REGS[PIPE_REG_IDEX.rs] < PIPE_REG_IDEX.imm;
}

void ori()
{
    // 0-extend immediate value
    PIPE_REG_EXMEM.data = 
        NEXT_STATE.REGS[PIPE_REG_IDEX.rs] | (0X0000FFFF & PIPE_REG_IDEX.imm);
}

// TODO
void lui()
{
    // load imm into upper 16 bits
    PIPE_REG_EXMEM.data = PIPE_REG_IDEX.imm << 16;
}

// TODO
void lw()
{
    PIPE_REG_EXMEM.data = 
        mem_read_32(CURRENT_STATE.REGS[PIPE_REG_IDEX.rs] + PIPE_REG_IDEX.imm);
}

// TODO
void sw()
{
    mem_write_32(CURRENT_STATE.REGS[PIPE_REG_IDEX.rs] + PIPE_REG_IDEX.imm, CURRENT_STATE.REGS[PIPE_REG_IDEX.rt]);
}

void pipe_stage_decode()
{
    uint32_t instruction = PIPE_REG_IFDE.instruction;
    if (!instruction) {
        RUN_BIT = 0; // TODO 
        return;
    }
    uint32_t opcode = instruction >> 26;
	if (opcode == 0)
		decode_r(instruction);
	else if (opcode == 2 || opcode == 3)
		decode_j(instruction);
	else if (opcode > 3)
		decode_i(instruction);
    PIPE_REG_IDEX.opcode = opcode;
}

void decode_r(uint32_t instruction)
{
    // Use masks to get parts of instruction, shift for next part
	PIPE_REG_IDEX.ALUop = instruction & 0x3F;
	instruction >>= 11;
	PIPE_REG_IDEX.rd = instruction & 0x1F;
	instruction >>= 5;
	PIPE_REG_IDEX.rt = instruction & 0x1F;
	instruction >>= 5;
	PIPE_REG_IDEX.rs = instruction & 0x1F;
}

void decode_j(uint32_t instruction)
{
    // Get 26 bits for instruction
	uint32_t addr = instruction & 0X3FFFFFF;
    // Final instruction shifted by 2, with 4 high bites from next pc
    addr <<= 2;
    PIPE_REG_IDEX.addr = ((CURRENT_STATE.PC + 4) & 0XF0000000) | addr;
}

void decode_i(uint32_t instruction)
{
    PIPE_REG_IDEX.imm = sign_extend(instruction & 0xFFFF);
	instruction >>= 16;
	PIPE_REG_IDEX.rt = instruction & 0x1F;
	instruction >>= 5;
	PIPE_REG_IDEX.rs = instruction & 0x1F;
	instruction >>= 5;
}


void pipe_stage_fetch()
{
    PIPE_REG_IFDE.instruction = mem_read_32(CURRENT_STATE.PC);
    printf("CURRENT_STATE: %d", CURRENT_STATE.PC);
    CURRENT_STATE.PC += 4;
}
