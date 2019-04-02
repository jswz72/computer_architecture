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

typedef struct Pipe_Reg_IFDE_Struct {
    uint32_t instruction;
    uint32_t PCval; // PCval to update
    uint32_t nop;   
    uint32_t nofetch;   // Nop for the fetch stage
} Pipe_Reg_IFDE;

typedef struct Pipe_Reg_IDEX_Struct {
    uint32_t ALUfunc; // For ALU control
    uint32_t opcode; // For main control of functionality in later stages
    uint32_t rs;    // Rs register (USED IN FWDING logic)
    uint32_t rt;    // Rt register (USED IN FWDING logic)
    int32_t reg_val1; // Read reg data 1 (value of rs)
    int32_t reg_val2; // Read reg data 2 (valud of rt)
    uint32_t dest;  // Destination register
    int32_t imm;   // For use in ALU with i-type
    uint32_t PCval; // Propagate new PC value
    uint32_t done;  // No more instructions left
    uint32_t nop;
} Pipe_Reg_IDEX;

typedef struct Pipe_Reg_EXMEM_Struct {
    uint32_t opcode; // For control of load/store in mem stage
    int32_t ALUresult; // Result of ALU computations (can be address depending on instruction)
    uint32_t dest; // Destination register (USED IN FWDING logic)
    int32_t mem_write_val;  // Value to write to memory
    int32_t reg_write;  // Whether or not register will be written
    uint32_t done;  // No more instructions left
    uint32_t rt;    // The rt register used (where mem_write_val comes from) (USED IN MEM-MEM FWDING)
    uint32_t nop;
} Pipe_Reg_EXMEM;

typedef struct Pipe_Reg_MEMWB_Struct {
    uint32_t data;  // Data to write to register
    uint32_t dest;  // Destination register (USED IN FWDING LOGIC)
    uint32_t reg_write;  // Controls whether data is written to dest register
    uint32_t done;  // No more instructions left
    uint32_t nop;
    // The register written to during the current cycle [because .data gets overwritten in the mem cycle] (USED IN FWDING)
    uint32_t reg_wrote;
    // The value written during the current cycle [because .data gets overwritten in the mem cycle] (USED IN FWDING)
    int32_t wrote_val;
} Pipe_Reg_MEMWB;

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
