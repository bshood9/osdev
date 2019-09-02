/* File: vga_text_mode.c
 *
 * Routines for drawing in VGA 80x25 16-color text-mode.
 *
 * Should replace Intel syntax with GNU assembler syntax for consistency
 * with later code
 */

#include "vga_text_mode.h"


void set_gbl_text_attr (uint8_t byte)
{
  gbl_text_attr = byte;
}

uint8_t get_gbl_text_attr ()
{
  return gbl_text_attr;
}

void clear_screen ()
{
  asm(
      ".intel_syntax noprefix\n"

      "xor al, al\n" 		/* null character */
      "mov ah, gbl_text_attr\n"	/* color attribute */
      "mov dx, ax\n"
      "shl eax, 16\n"
      "mov ax, dx\n"

      "mov edi, 0xb8000\n"
      "mov ecx, 1000\n"		/* 4 bytes at a time * 1000 times = 4000 */
      "cld\n"
      "rep stosd\n"

      ".att_syntax\n"
      );
}

void write_str (char* str, uint32_t size,
		uint32_t x_coord, uint32_t y_coord)
{
  asm(
      ".intel_syntax noprefix\n"

      "mov esi, [ebp+8]\n"

      "mov eax, [ebp+20]\n"
      "mov edi, 160\n"
      "mul edi\n"
      "add eax, [ebp+16]\n"
      "add eax, [ebp+16]\n"
      "add eax, 0xb8000\n"
      "mov edi, eax\n"

      "mov ecx, [ebp+12]\n"

      "mov ah, gbl_text_attr\n"

      "_loop:\n"
      "lodsb\n"
      "stosw\n"
      "loop _loop\n"

      ".att_syntax\n"
      );
}

/* unfinished */
void write_rectangle (uint32_t x_coord, uint32_t y_coord,
		      uint32_t length, uint32_t width,
		      char x, char y, char corner)
{
  asm(
      ".intel_syntax noprefix\n"

      "mov eax, [ebp+12]\n"	/* y_coord */
      "mov edx, 160\n"
      "mul edx\n"
      "mov edx, [ebp+8]\n"	/* x_coord */
      "shl edx, 1\n"
      "add eax, edx\n"
      "add eax, 0xb8000\n"
      "mov edi, eax\n"		/* edi = destination address */

      "mov eax, [ebp+16]\n"	/* length */
      "mov edx, [ebp+20]\n"	/* width */

      "\n"
      "\n"
      "\n"
      "\n"
      "\n"
      "\n"
      "\n"
      "\n"
      "\n"
      "\n"
      "\n"
      "\n"

      ".att_syntax\n"
      );
}
