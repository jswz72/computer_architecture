#ifndef _PIPE_H_
#define _PIPE_H_

#include "shell.h"
#include "stdbool.h"
#include <limits.h>

#define MIPS_REGS 32

typedef struct CPU_State_Struct {
  uint32_t PC;		/* program counter */
  int32_t REGS[MIPS_REGS]; /* register file. */
  int FLAG_N;        /* negative flag or sign flag*/
  int FLAG_Z;        /* zero flag */
  int FLAG_V;        /* overflow flag */
  int FLAG_C;        /* carry flag */
} CPU_State;

/**
 * Pipeline register between fetch and decode stages
 * instruction: the hex instruction
 * op: ALU operation
 * rs: operand register
 * rt: operand register
 * rd: destination register
 **/
typedef struct Pipe_Reg_IFDE_Struct {
    uint32_t instruction;
} Pipe_Reg_IFDE;

/**
 * Pipeline register between decode and execute stages
 * ALUop: ALU operation
 * rs: first decoded opearand
 * rt: second decoded operand
 * rd: result destination
 **/
typedef struct Pipe_Reg_IDEX_Struct {
    uint32_t ALUop;
    uint32_t opcode; // TODO
    int32_t rs;
    int32_t rt;
    uint32_t rd;
    uint32_t addr;  //TODO
    int32_t imm;   // TODO
} Pipe_Reg_IDEX;

/**
 * Pipeline register between decode and execute stages
 * data: results of execution
 * dest: result destination
 **/
typedef struct Pipe_Reg_EXMEM_Struct {
    int32_t data;
    int32_t offset;
    uint32_t dest;
} Pipe_Reg_EXMEM;

int RUN_BIT;

/* global variable -- pipeline state */
extern CPU_State CURRENT_STATE;

/* called during simulator startup */
void pipe_init();

/* this function calls the others */
void pipe_cycle();

/* each of these functions implements one stage of the pipeline */
void pipe_stage_fetch();
void pipe_stage_decode();
void pipe_stage_execute();
void pipe_stage_mem();
void pipe_stage_wb();

#endif
