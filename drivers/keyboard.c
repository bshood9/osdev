/* File: keyboard.c
 *
 * Driver for keyboard
 * Currently incomplete
 */

#include "keyboard.h"

char scancode_buffer[SCANCODE_BUFFER_MAX_SIZE];
int buffer_size = 0;
char* msg = "Welcome to the botnet";
int line = 0;


/* Intended to be called by the interrupt handler
 * PS2 controller has data waiting to be read
 */
static void read_ps2_output_buffer ()
{
  uint8_t scancode;

  scancode = inb(0x60);

  switch (scancode)
    {
    case 0xe0:			/* 2-byte scancode */
      {
	switch (inb(0x60))
	  {	
	  case 0xf0:
	    break;
	  }

	break;
      }

    case 0xf0:			/* break code */
      break;			/* we fall out in all cases */
				/* because this is not complete */
    }
}

void keyboard_interrupt ()
{
  uint8_t scancode = inb(0x60);

  write_str(msg, strlen(msg), 0, line++);

/* this tells the PIC that we are done with the interrupt */
  outb(PIC_MASTER_CMD_REG, 0x20); 

  asm("leave");
  asm(".byte 0xcb");
}
