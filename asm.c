/* File: asm.c
 *
 * Convenience to avoid inline assembly; not currently used 
 */

#include "asm.h"

static char nop_instruction[] = {NOP_OPCODE};
static char ret_instruction[] = {RETN_OPCODE};
static char hlt_instruction[] = {HLT_OPCODE};
static char int_instruction[] = {INT_OPCODE, 0x00};
static char sti_instruction[] = {STI_OPCODE};

void (*nop)(void) = (void*)nop_instruction;
void (*ret)(void) = (void*)ret_instruction;
void (*hlt)(void) = (void*)hlt_instruction;
void (*itr)(void) = (void*)int_instruction;
void (*sti)(void) = (void*)sti_instruction;
