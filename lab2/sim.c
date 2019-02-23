#include <stdio.h>
#include "shell.h"

enum InstructType {R, I, J};

// General
static uint32_t instruction;
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

void fetch()
{
    instruction = mem_read_32(CURRENT_STATE.PC);
	if (instruction == 0) {
		RUN_BIT = 0;
		return;
	}
}

void decode_rtype()
{
	funct = instruction & 0x3F; // 6 ones
	instruction >>= 11;
	rd = instruction & 0x1F; //5 ones
	instruction >>= 5;
	rt = instruction & 0x1F; //5 ones
	instruction >>= 5;
	rs = instruction & 0x1F; //5 ones
}

void decode_jtype()
{
	addr = instruction & 0X3FFFFFF;
}

void decode_itype()
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
    uint32_t opcode = instruction >> 26;
	if (opcode == 0) {
		instruct_type = R;
		decode_rtype();
	}
	else if (opcode == 2 || opcode == 3) {
		instruct_type = J;
		decode_jtype();
	}
	else if (opcode > 3) {
		instruct_type = I;
		decode_itype();
	}
}

void execute_r()
{
}

void execute_i()
{
}

void execute()
{
	if (instruct_type == R)
		execute_r();
	if (instruct_type == J)
		NEXT_STATE.PC = addr;
	if (instruct_type == I)
		execute_i();
    if (instruct_type != J)
		NEXT_STATE.PC = CURRENT_STATE.PC + 4;
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
