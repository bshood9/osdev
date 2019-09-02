/* File: pit.c
 *
 * Functions for controlling the 8253 Programmable Interval Timer (PIT).
 */

#include "pit.h"


void pit_send_cmd (uint8_t cmd)
{
  outb(PIT_CTRL_WORD_REG, cmd);
}

void pit_send_count (int counter_port, int cycles)
{
  outb(counter_port, cycles & 0xff);
  outb(counter_port, cycles >> 8);
}
