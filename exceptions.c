/* File: exceptions.c
 *
 * Interrupt service routines for hardware exceptions.
 */

#include "exceptions.h"

/* IMPORTANT NOTE:
 *
 * Compiler may insert "leave, retn" instructions after the inline asm;
 * we cannot use those however, as the ISR wrapper function makes a FAR
 * call to this routine. Why does it make a far call? The ISR wrapper
 * needs to call an ABSOLUTE, IMMEDIATE address; this cannot be achieved
 * with a near call.
 */


/*
 * The slot in the interrupt table is indicated by the comment
 * preceding each function.
 */

/* int 0x00 */
void divide_by_zero_isr ()
{
  char* error_msg = "CPU Exception: Divide by zero error";
  set_gbl_text_attr(0xf0); /* sets text foreground and background colors */
  write_str(error_msg, strlen(error_msg), 0, 24); /* strlen about 280 instr */


  asm("hlt");		  /* TODO: replace with error-handling code */


  asm("leave");
  asm(".byte 0xcb");		/* return far to calling procedure */
}

/* int 0x02 */
void nmi_interrupt_isr ()
{
  char* error_msg = "CPU Exception: NMI Interrupt received";
  set_gbl_text_attr(0xf0);
  write_str(error_msg, strlen(error_msg), 0, 24);


  asm("hlt");		  /* TODO: replace with error-handling code */


  asm("leave");
  asm(".byte 0xcb");		/* return far to calling procedure */
}

/* int 0x03 */
void breakpoint_isr ()
{
  char* error_msg = "CPU Exception: Breakpoint encountered";
  set_gbl_text_attr(0xf0);
  write_str(error_msg, strlen(error_msg), 0, 24);


  asm("hlt");		  /* TODO: replace with error-handling code */


  asm("leave");
  asm(".byte 0xcb");		/* return far to calling procedure */
}

/* int 0x04 */
void overflow_isr ()
{
  char* error_msg = "CPU Exception: Overflow";
  set_gbl_text_attr(0xf0);
  write_str(error_msg, strlen(error_msg), 0, 24);


  asm("hlt");		  /* TODO: replace with error-handling code */


  asm("leave");
  asm(".byte 0xcb");		/* return far to calling procedure */
}

/* int 0x05 */
void bounds_range_exceeded_isr ()
{
  char* error_msg = "CPU Exception: Bounds Range Exceeded";
  set_gbl_text_attr(0xf0);
  write_str(error_msg, strlen(error_msg), 0, 24);


  asm("hlt");		  /* TODO: replace with error-handling code */


  asm("leave");
  asm(".byte 0xcb");		/* return far to calling procedure */
}

/* int 0x06 */
void invalid_opcode_isr ()
{
  char* error_msg = "CPU Exception: Invalid Opcode";
  set_gbl_text_attr(0xf0);
  write_str(error_msg, strlen(error_msg), 0, 24);


  asm("hlt");		  /* TODO: replace with error-handling code */


  asm("leave");
  asm(".byte 0xcb");		/* return far to calling procedure */
}

/* int 0x07 */
void device_unavailable_isr ()
{
  char* error_msg = "CPU Exception: Device unavailable";
  set_gbl_text_attr(0xf0);
  write_str(error_msg, strlen(error_msg), 0, 24);


  asm("hlt");		  /* TODO: replace with error-handling code */


  asm("leave");
  asm(".byte 0xcb");		/* return far to calling procedure */
}

/* int 0x08 */
void double_fault_isr ()
{
  char* error_msg = "CPU Exception: Double Fault encountered";
  set_gbl_text_attr(0xf0);
  write_str(error_msg, strlen(error_msg), 0, 24);


  asm("hlt");		  /* TODO: replace with error-handling code */

  /* CPU pushes error code */

  asm("leave");
  asm(".byte 0xcb");		/* return far to calling procedure */
}

/* int 0x09 */
void coprocessor_segment_overrun_isr ()
{
  char* error_msg = "CPU Exception: Co-processor Segment Overrun";
  set_gbl_text_attr(0xf0);
  write_str(error_msg, strlen(error_msg), 0, 24);


  asm("hlt");		  /* TODO: replace with error-handling code */


  asm("leave");
  asm(".byte 0xcb");		/* return far to calling procedure */
}

/* int 0x0a */
void invalid_tss_isr ()
{
  char* error_msg = "CPU Exception: Invalid TSS";
  set_gbl_text_attr(0xf0);
  write_str(error_msg, strlen(error_msg), 0, 24);


  asm("hlt");		  /* TODO: replace with error-handling code */

  /* CPU pushes error code */

  asm("leave");
  asm(".byte 0xcb");		/* return far to calling procedure */
}

/* int 0x0b */
void segment_not_present_isr ()
{
  char* error_msg = "CPU Exception: Segment Not Present";
  set_gbl_text_attr(0xf0);
  write_str(error_msg, strlen(error_msg), 0, 24);


  asm("hlt");		  /* TODO: replace with error-handling code */


  asm("leave");
  asm(".byte 0xcb");		/* return far to calling procedure */
}

/* int 0x0c */
void stack_segment_fault_isr ()
{
  char* error_msg = "CPU Exception: Stack Segment Fault";
  set_gbl_text_attr(0xf0);
  write_str(error_msg, strlen(error_msg), 0, 24);


  asm("hlt");		  /* TODO: replace with error-handling code */


  asm("leave");
  asm(".byte 0xcb");		/* return far to calling procedure */
}

/* int 0x0d */
void general_protection_fault_isr ()
{
  char* error_msg = "CPU Exception: General Protection Fault";
  set_gbl_text_attr(0xf0);
  write_str(error_msg, strlen(error_msg), 0, 24);


  asm("hlt");		  /* TODO: replace with error-handling code */


  asm("leave");
  asm(".byte 0xcb");		/* return far to calling procedure */
}

/* int 0x0e */
void page_fault_isr ()
{
  char* error_msg = "CPU Exception: Page Fault";
  set_gbl_text_attr(0xf0);
  write_str(error_msg, strlen(error_msg), 0, 24);


  asm("hlt");		  /* TODO: replace with error-handling code */

  /* CPU pushes error code */

  asm("leave");
  asm(".byte 0xcb");		/* return far to calling procedure */
}

/* int 0x10 */
void x87_fpu_error_isr ()
{
  char* error_msg = "CPU Exception: x87 FPU Error";
  set_gbl_text_attr(0xf0);
  write_str(error_msg, strlen(error_msg), 0, 24);

  
  asm("hlt");		  /* TODO: replace with error-handling code */


  asm("leave");
  asm(".byte 0xcb");		/* return far to calling procedure */
}

/* int 0x11 */
void alignment_check_isr ()
{
  char* error_msg = "CPU Exception: Alignment Check";
  set_gbl_text_attr(0xf0);
  write_str(error_msg, strlen(error_msg), 0, 24);


  asm("hlt");		  /* TODO: replace with error-handling code */


  asm("leave");
  asm(".byte 0xcb");		/* return far to calling procedure */
}

/* int 0x12 */
void machine_check_isr ()
{
  char* error_msg = "CPU Exception: Machine Check";
  set_gbl_text_attr(0xf0);
  write_str(error_msg, strlen(error_msg), 0, 24);


  asm("hlt");		  /* TODO: replace with error-handling code */


  asm("leave");
  asm(".byte 0xcb");		/* return far to calling procedure */
}

/* int 0x13 */
void simd_float_exception_isr ()
{
  char* error_msg = "CPU Exception: SIMD Floating-point Exception";
  set_gbl_text_attr(0xf0);
  write_str(error_msg, strlen(error_msg), 0, 24);


  asm("hlt");		  /* TODO: replace with error-handling code */


  asm("leave");
  asm(".byte 0xcb");		/* return far to calling procedure */
}

void idt_add_exceptions ()
{
  idt_install_interrupt(0x00, &divide_by_zero_isr, 0x8e);
  idt_install_interrupt(0x01, 0x00, 0x00); /* reserved */
  idt_install_interrupt(0x02, &nmi_interrupt_isr, 0x8e);
  idt_install_interrupt(0x03, &breakpoint_isr, 0x8e);
  idt_install_interrupt(0x04, &overflow_isr, 0x8e);
  idt_install_interrupt(0x05, &bounds_range_exceeded_isr, 0x8e);
  idt_install_interrupt(0x06, &invalid_opcode_isr, 0x8e);
  idt_install_interrupt(0x07, &device_unavailable_isr, 0x8e);
  idt_install_interrupt(0x08, &double_fault_isr, 0x8e);
  idt_install_interrupt(0x09, &coprocessor_segment_overrun_isr, 0x8e);
  idt_install_interrupt(0x0a, &invalid_tss_isr, 0x8e);
  idt_install_interrupt(0x0b, &segment_not_present_isr, 0x8e);
  idt_install_interrupt(0x0c, &stack_segment_fault_isr, 0x8e);
  idt_install_interrupt(0x0d, &general_protection_fault_isr, 0x8e);
  idt_install_interrupt(0x0e, &page_fault_isr, 0x8e);
  idt_install_interrupt(0x0f, 0x00, 0x00); /* reserved */
  idt_install_interrupt(0x10, &x87_fpu_error_isr, 0x8e);
  idt_install_interrupt(0x11, &alignment_check_isr, 0x8e);
  idt_install_interrupt(0x12, &machine_check_isr, 0x8e);
  idt_install_interrupt(0x13, &simd_float_exception_isr, 0x8e);

  //  idt_install_interrupt(0x14, 0, 0); /* reserved */
  //  idt_install_interrupt(0x15, 0, 0); /* reserved */
  //  idt_install_interrupt(0x16, 0, 0); /* reserved */
  //  idt_install_interrupt(0x17, 0, 0); /* reserved */
  //  idt_install_interrupt(0x18, 0, 0); /* reserved */
  //  idt_install_interrupt(0x19, 0, 0); /* reserved */
  //  idt_install_interrupt(0x1a, 0, 0); /* reserved */
  //  idt_install_interrupt(0x1b, 0, 0); /* reserved */
  //  idt_install_interrupt(0x1c, 0, 0); /* reserved */
  //  idt_install_interrupt(0x1d, 0, 0); /* reserved */
  //  idt_install_interrupt(0x1e, 0, 0); /* reserved */

  idt_install_interrupt(0x1f, 0, 0); /* reserved */
}
