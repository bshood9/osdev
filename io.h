/* File: io.h
 *
 * Routines to read and write from/to an IO mapped device
 */

#include <stdint.h>

#ifndef IO_H
#define IO_H


static inline void outb (uint16_t port, uint8_t byte)
{
  /* it may be that volatile is required after asm and before the
   * parenthesis if it is to apply to the variables in the
   * assembly code
   */
  asm volatile("movw %0, %%dx\n\t"
	       "movb %1, %%al\n\t"
	       "out %%al, %%dx\n\t"

	       :
	       : "rm" (port), "rm" (byte)
	       : "al", "dx");
}

static inline void outw (uint16_t port, uint16_t word)
{
  asm volatile("movw %0, %%dx\n\t"
	       "movw %1, %%ax\n\t"
	       "out %%ax, %%dx\n\t"

	       :
	       : "rm" (port), "rm" (word)
	       : "ax", "dx");
}

static inline void outl (uint16_t port, uint32_t dword)
{
  asm volatile("movw %0, %%dx\n\t"
	       "movl %1, %%eax\n\t"
	       "out %%eax, %%dx\n\t"

	       :
	       : "rm" (port), "rm" (dword)
	       : "eax", "dx");
}

static inline uint8_t inb (uint16_t port)
{
  uint8_t byte;

  asm volatile("movw %1, %%dx\n\t"
	       "in %%dx, %%al\n\t"
	       "movb %%al, %0\n\t"

	       : "=rm" (byte)
	       : "rm" (port)
	       : "al", "dx");

  return byte;
}

static inline uint16_t inw (uint16_t port)
{
  uint16_t word;

  asm volatile("movw %1, %%dx\n\t"
	       "in %%dx, %%ax\n\t"
	       "movw %%ax, %0\n\t"

	       : "=rm" (word)
	       : "rm" (port)
	       : "ax", "dx");

  return word;
}

static inline uint32_t inl (uint16_t port)
{
  uint32_t dword;

  asm volatile("movw %1, %%dx\n\t"
	       "in %%dx, %%eax\n\t"
	       "movl %%eax, %0\n\t"

	       : "=rm" (dword)
	       : "rm" (port)
	       : "eax", "dx");

  return dword;
}


#endif	/* IO_H */
