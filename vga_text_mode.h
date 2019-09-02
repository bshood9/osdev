/* File: vga_text_mode.h
 *
 * Header file with function declarations for VGA 80x25 16-color text-mode
 * routines.
 */

#include <stdint.h>

#ifndef VGA_TEXT_MODE_H
#define VGA_TEXT_MODE_H

#define BASE_VIDEO_RAM      0xa0000
#define TEXT_MODE_VIDEO_RAM 0xb8000

static uint8_t gbl_text_attr;
static uint8_t cursor_x;
static uint8_t cursor_y;


void set_gbl_text_attr (uint8_t byte);
uint8_t get_gbl_text_attr ();
void clear_screen ();
void write_str (char* str, uint32_t size,
		uint32_t x_coord, uint32_t y_coord);


#endif	/* VGA_TEXT_MODE_H */
