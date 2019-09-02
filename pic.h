/* File: pic.h
 *
 * Header file for functions that communicate with the 8259 PIC
 */

#include <stdint.h>
#include "io.h"

#ifndef PIC_8259_H
#define PIC_8259_H

#define PIC_MASTER_CMD_REG    0x20 /* accessed on write */
#define PIC_MASTER_STATUS_REG 0x20 /* accessed on read */
#define PIC_MASTER_DATA_REG   0x21 /* accessed on read */
#define PIC_MASTER_IMR_REG    0x21 /* accessed on write */

#define PIC_SLAVE_CMD_REG    0xa0 /* accessed on write */
#define PIC_SLAVE_STATUS_REG 0xa0 /* accessed on read */
#define PIC_SLAVE_DATA_REG   0xa1 /* accessed on read */
#define PIC_SLAVE_IMR_REG    0xa1 /* accessed on write */


/* Initialization Control Word 1 (ICW 1):
 *
 *      +-----+-----+-----+------+------+------+------+------+
 *      |  0  |  0  |  0  | INIT | LTIM | ADIV | SNGL | ICW4 |
 *      +-----+-----+-----+------+------+------+------+------+
 * bit:       7     6     5      4      3      2      1      0
 *
 * INIT: initialization bit
 * LTIM: level-triggered / edge-triggered
 * ADIV: address interval 4 / 8; usually ignored by x86
 * SNGL: single / cascading PICs (if cascading, need ICW3)
 * ICW4: expect to receive ICW4
 */

void send_icw1 ();

/* ICW2 specifies offset into IDT to map IRQs. */

void send_icw2 ();

/* ICW3 specifies what IRQ lines the two PICs use to communicate.
   Bitmap for the master PIC, an actual number for the slave PIC. */

void send_icw3 ();

/* ICW4:
 *
 *      +------+------+------+------+-----+-----+------+-----+
 *      | rsvd | rsvd | rsvd | SFNM | BUF | M/S | AEOI | uPM |
 *      +------+------+------+------+-----+-----+------+-----+
 * bit:        7      6      5      4     3     2      1     0
 *
 * Reserved: bits 5-7
 * SFNM: Special Fully Nested Mode -> used in systems with many cascaded PICs
 * BUF:  operate in buffered mode
 * M/S:  select buffer master / slave (only use if BUF is set)
 * AEOI: perform EOI automatically on last interrupt ACK pulse ("rarely used")
 * uPM:  80x86 mode / MCS-80/86 mode
 */

void send_icw4 ();

/* Enable/disable IRQs */

void pic_disable_irqs (uint16_t bitmap);


#endif	/* PIC_8259_H */
