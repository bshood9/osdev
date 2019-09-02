/* File: pic.c
 *
 * Functions for initializing and communicating with the 8259
 * Programmable Interrupt Controller (PIC).
 */

#include "pic.h"


void send_icw1 ()
{
  outb(PIC_MASTER_CMD_REG, 0x11);	/* binary -> 0001 0001 */
  outb(PIC_SLAVE_CMD_REG, 0x11);
}

/* ICW2 specifies offset into IDT to map IRQs. */

void send_icw2 ()
{
  outb(PIC_MASTER_DATA_REG, 0x20);
  outb(PIC_SLAVE_DATA_REG, 0x28);
}

/* ICW3 specifies what IRQ lines the two PICs use to communicate.
   ICW3 for the master PIC is a bitmap, and numerical for the slave. */

void send_icw3 ()
{
  outb(PIC_MASTER_DATA_REG, 0x04);	/* IRQ bitmap: 0000 0100 = 0x04 */
  outb(PIC_SLAVE_DATA_REG, 0x02);	/* IRQ = 2 */
}

void send_icw4 ()
{
  outb(PIC_MASTER_DATA_REG, 0x01);
  outb(PIC_SLAVE_DATA_REG, 0x01);

  /* clear the data registers (dunno if necessary) */

  outb(PIC_MASTER_DATA_REG, 0x00);
  outb(PIC_SLAVE_DATA_REG, 0x00);
}

void pic_disable_irqs (uint16_t bitmap)
{
  outb(PIC_SLAVE_IMR_REG, bitmap >> 8); /* disable low priority IRQs first */
  outb(PIC_MASTER_IMR_REG, bitmap & 0xff);
}
