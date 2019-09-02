/* File: exceptions.h
 *
 * Header file with function declarations for hardware exception ISRs.
 */

#include "lib/string.h"
#include "vga_text_mode.h"
#include "idt.h"

#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

/* Only idt_add_exceptions() is expected to be called from
 * outside of exceptions.c, so the other function prototypes
 * could be removed from here.
 */

/* int 0x00 */
void divide_by_zero_isr ();

/* int 0x02 */
void nmi_interrupt_isr ();

/* int 0x03 */
void breakpoint_isr ();

/* int 0x04 */
void overflow_isr ();

/* int 0x05 */
void bounds_range_exceeded_isr ();

/* int 0x06 */
void invalid_opcode_isr ();

/* int 0x07 */
void device_unavailable_isr ();

/* int 0x08 */
void double_fault_isr ();

/* int 0x09 */
void coprocessor_segment_overrun_isr ();

/* int 0x0a */
void invalid_tss_isr ();

/* int 0x0b */
void segment_not_present_isr ();

/* int 0x0c */
void stack_segment_fault_isr ();

/* int 0x0d */
void general_protection_fault_isr ();

/* int 0x0e */
void page_fault_isr ();

/* int 0x10 */
void x87_fpu_error_isr ();

/* int 0x11 */
void alignment_check_isr ();

/* int 0x12 */
void machine_check_isr ();

/* int 0x13 */
void simd_float_exception_isr ();

void idt_add_exceptions ();


#endif	/* EXCEPTIONS_H */
