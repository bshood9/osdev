/* File: asm.h
 *
 * Header file containing hard-coded function-pointers to asm instructions
 * and macros for x86 instructions.
 */

#ifndef ASM_H
#define ASM_H

#define PUSH_EAX         0x50
#define PUSH_ECX         0x51
#define PUSH_EDX         0x52
#define PUSH_EBX         0x53
#define PUSH_ESP         0x54
#define PUSH_EBP         0x55
#define PUSH_ESI         0x56
#define PUSH_EDI         0x57

#define POP_EAX          0x58
#define POP_ECX          0x59
#define POP_EDX          0x5a
#define POP_EBX          0x5b
#define POP_ESP          0x5c
#define POP_EBP          0x5d
#define POP_ESI          0x5e
#define POP_EDI          0x5f

#define PUSHAD_OPCODE    0x60	/* same as pusha */
#define POPAD_OPCODE     0x61	/* same as popa */

#define MOV_EBP_ESP      0x89e5
#define NOP_OPCODE       0x90
#define CALL_FAR_OPCODE  0x9a
#define RETN_OPCODE      0xc3
#define LEAVE_OPCODE     0xc9
#define RETF_OPCODE      0xcb
#define INT_OPCODE       0xcd
#define IRET_OPCODE      0xcf	/* same as iretd */
#define HLT_OPCODE       0xf4
#define STI_OPCODE       0xfb
#define CLD_OPCODE       0xfc
#define CALL_OPCODE      0xff	/* ff /2 = call r/m32; ff /3 = call m16:32 */

void (*nop)(void);
void (*ret)(void);
void (*hlt)(void);
void (*itr)(void);
void (*sti)(void);


#endif	/* ASM_H */
