#include <stdio.h>
#include "shell.h"

static uint32_t CUR_OPCODE;

void fetch()
{
    uint32_t pc_content = mem_read_32(CURRENT_STATE.PC);
    CUR_OPCODE = pc_content >> 26;
    printf("cur opcode: %d", CUR_OPCODE);
}

void decode()
{
    
}

void execute()
{
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
