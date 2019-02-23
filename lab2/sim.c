#include <stdio.h>
#include "shell.h"

enum InstructType {R, I, J};

// General
static uint32_t instruction;
static uint32_t opcode;
enum InstructType instruct_type;

// R type or i type
static uint32_t rs;
static uint32_t rt;

// R type
static uint32_t funct;
static uint32_t rd;

// I type
static uint32_t imm;

// J type
static uint32_t addr;

void inc_pc(int offset)
{
	NEXT_STATE.PC = CURRENT_STATE.PC + 4 + offset;
}

void fetch()
{
    instruction = mem_read_32(CURRENT_STATE.PC);
	if (instruction == 0) {
		RUN_BIT = 0;
		return;
	}
}

void decode_r()
{
	funct = instruction & 0x3F; // 6 ones
	instruction >>= 11;
	rd = instruction & 0x1F; //5 ones
	instruction >>= 5;
	rt = instruction & 0x1F; //5 ones
	instruction >>= 5;
	rs = instruction & 0x1F; //5 ones
}

void decode_j()
{
	addr = instruction & 0X3FFFFFF;
}

void decode_i()
{
	imm = instruction & 0xFFFF;
	imm >>= 16;
	rt = instruction & 0x1F;
	instruction >>= 5;
	rs = instruction & 0x1F;
	instruction >>= 5;
}


void decode()
{
    opcode = instruction >> 26;
	if (opcode == 0) {
		instruct_type = R;
		decode_r();
	}
	else if (opcode == 2 || opcode == 3) {
		instruct_type = J;
		decode_j();
	}
	else if (opcode > 3) {
		instruct_type = I;
		decode_i();
	}
}

// todo signed vs unsigned?
// todo flags
void add() 
{
	// todo check and set overflow
	NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] + CURRENT_STATE.REGS[rt];
}
void addu() 
{
	NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] + CURRENT_STATE.REGS[rt];
}
void sub() 
{
	// todo check and set over(under)flow
	NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] - CURRENT_STATE.REGS[rt];
}
void subu() 
{
	NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] - CURRENT_STATE.REGS[rt];
}
void slt() 
{
	NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] < CURRENT_STATE.REGS[rt];
}
void sltu() 
{
	// todo
}

void execute_r()
{
	switch(funct) {
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
}


void beq()
{
	// todo sign extend?
	int offset = 0;
	if (CURRENT_STATE.REGS[rs] == CURRENT_STATE.REGS[rt])
		offset = imm;
	inc_pc(offset);
}
void bne()
{
	// todo sign extend?
	int offset = 0;
	if (CURRENT_STATE.REGS[rs] != CURRENT_STATE.REGS[rt])
		offset = imm;
	inc_pc(offset);
}
void bgtz()
{
	int offset = 0;
	if (CURRENT_STATE.REGS[rs] > 0)
		offset = imm;
	inc_pc(offset);
}
void addi()
{
}
void addiu()
{
}
void slti()
{
}
void ori()
{
}
void lui()
{
}
void lw()
{
}
void sw()
{
}

void execute_i()
{
	switch(opcode) {
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
		case 34:
			lw();
			break;
		case 43:
			sw();
			break;
	}
	if (opcode > 7)
		inc_pc(0);
}

void execute()
{
	if (instruct_type == R)
		execute_r();
	if (instruct_type == J)
		NEXT_STATE.PC = addr;
	if (instruct_type == I)
		execute_i();
}

void process_instruction()
{
    /* execute one instruction here. You should use CURRENT_STATE and modify
     * values in NEXT_STATE. You can call mem_read_32() and mem_write_32() to
     * access memory. */
    fetch();
    decode();
    execute();

}
