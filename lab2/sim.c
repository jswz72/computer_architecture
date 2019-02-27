#include <stdio.h>
#include "shell.h"

// todo carry flag

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
}

void decode_r()
{
	funct = instruction & 0x3F;
	instruction >>= 11;
	rd = instruction & 0x1F;
	instruction >>= 5;
	rt = instruction & 0x1F;
	instruction >>= 5;
	rs = instruction & 0x1F;
}

void decode_j()
{
    // Get 26 bits for instruction
	addr = instruction & 0X3FFFFFF;
    // Final instruction shifted by 2, with 4 high bites from next pc
    addr <<= 2;
    addr = ((CURRENT_STATE.PC + 4) & 0XF0000000) | addr;
}

int32_t sign_extend(int16_t a)
{
    int mask = 0x8000;
    if (mask & a) {
        a = a | 0xFFFF0000;
    }
    return a;
}

void decode_i()
{
    imm = sign_extend(instruction & 0xFFFF);
	instruction >>= 16;
	rt = instruction & 0x1F;
	instruction >>= 5;
	rs = instruction & 0x1F;
	instruction >>= 5;
}


void decode()
{
    if (!instruction) {
        RUN_BIT = 0;
        return;
    }
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

void check_overflow(int32_t a, int32_t b, int32_t res) {
    if (a > 0 && b > 0 && res < 0
            || a < 0 && b < 0 && res > 0) {
        NEXT_STATE.FLAG_V = 1;
    }
}

void add() 
{
    int32_t rs_val = CURRENT_STATE.REGS[rs];
    int32_t rt_val = CURRENT_STATE.REGS[rt];
    int32_t res = rs_val + rt_val;
    check_overflow(rs_val, rt_val, res);
    NEXT_STATE.REGS[rd] = res;
    if (NEXT_STATE.REGS[rd] < 0)
        NEXT_STATE.FLAG_N = 1;

}

void addu() 
{
    uint32_t rs_val = CURRENT_STATE.REGS[rs];
    uint32_t rt_val = CURRENT_STATE.REGS[rt];
    unsigned int res = rs_val + rt_val;
    if (res > UINT32_MAX)
        NEXT_STATE.FLAG_C = 1;
	NEXT_STATE.REGS[rd] = (uint32_t) res;
}

void sub() 
{
    int32_t rs_val = CURRENT_STATE.REGS[rs];
    int32_t rt_val = CURRENT_STATE.REGS[rt];
    int32_t res = rs_val - rt_val;
    check_overflow(rs_val, rt_val, res);
    if (res < 0)
        NEXT_STATE.FLAG_N = 1;
    NEXT_STATE.REGS[rd] = res;
}

void subu() 
{
    uint32_t rs_val = CURRENT_STATE.REGS[rs];
    uint32_t rt_val = CURRENT_STATE.REGS[rt];
    int res = rs_val - rt_val;
    if (res < INT32_MIN)
        NEXT_STATE.FLAG_C = 1;
    if (res < 0)
        NEXT_STATE.FLAG_N = 1;
	NEXT_STATE.REGS[rd] = res;
}

void slt() 
{
	NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] < CURRENT_STATE.REGS[rt];
}

void sltu() 
{
    uint32_t rs_val = CURRENT_STATE.REGS[rs];
    uint32_t rt_val = CURRENT_STATE.REGS[rt];
	NEXT_STATE.REGS[rd] = rs_val < rt_val;
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
    if (!NEXT_STATE.REGS[rd])
        NEXT_STATE.FLAG_Z = 1;
}




void beq()
{
	int offset = 0;
	if (CURRENT_STATE.REGS[rs] == CURRENT_STATE.REGS[rt])
		offset = imm * 4; // todo check
	inc_pc(offset);
}

void bne()
{
	int offset = 0;
	if (CURRENT_STATE.REGS[rs] != CURRENT_STATE.REGS[rt])
		offset = imm * 4; // todo check
	inc_pc(offset);
}

void bgtz()
{
	int offset = 0;
	if (CURRENT_STATE.REGS[rs] > 0)
		offset = imm * 4; // todo check
	inc_pc(offset);
}

void addi()
{
    int32_t rs_val = CURRENT_STATE.REGS[rs];
    int32_t res = rs_val + imm;
    check_overflow(rs_val, imm, res);
    NEXT_STATE.REGS[rt] = res;
    if (!res)
        NEXT_STATE.FLAG_Z = 1;
    if (res < 0)
        NEXT_STATE.FLAG_N = 1;
}

void addiu()
{
    uint32_t rs_val = CURRENT_STATE.REGS[rs];
    int res = rs_val + (uint32_t) imm;
    if (res > UINT32_MAX)
        NEXT_STATE.FLAG_C = 1;
    if (!res)
        NEXT_STATE.FLAG_Z = 1;
	NEXT_STATE.REGS[rt] = res;
}

void slti()
{
    NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] < imm;
}

void ori()
{
    // 0-extend imm
    NEXT_STATE.REGS[rt] = NEXT_STATE.REGS[rs] | (0X0000FFFF & imm);
    if (!NEXT_STATE.REGS[rt])
        NEXT_STATE.FLAG_Z = 1;
}

void lui()
{
    NEXT_STATE.REGS[rt] = imm << 16;
    if (!NEXT_STATE.REGS[rt])
        NEXT_STATE.FLAG_Z = 1;

}

void lw()
{
    NEXT_STATE.REGS[rt] = mem_read_32(CURRENT_STATE.REGS[rs] + imm);
    if (!NEXT_STATE.REGS[rt])
        NEXT_STATE.FLAG_Z = 1;
}

void sw()
{
    mem_write_32(CURRENT_STATE.REGS[rs] + imm, CURRENT_STATE.REGS[rt]);
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

void clear_flags()
{
    NEXT_STATE.FLAG_N = 0;
    NEXT_STATE.FLAG_Z = 0;
    NEXT_STATE.FLAG_V = 0;
    NEXT_STATE.FLAG_C = 0;
}

void execute()
{
    if (!RUN_BIT)
        return;
    clear_flags();
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
