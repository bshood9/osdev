/* File: pit.h
 *
 * Header file defining protocol for communication with the
 * 8253 Programmable Interval Timer (PIT).
 */

#include <stdint.h>
#include "io.h"
#include "pic.h"
#include "time.h"
#include "lib/list.h"

#ifndef PIT_8253_H
#define PIT_8253_H

#define PIT_MHZ_NUMERATOR 105
#define PIT_MHZ_DENOMINATOR 88

/* PIT_SYSTIME_COUNT is the number of oscillations of the PIT's internal clock
 * to wait for before raising the interrupt line connected to the PIC.
 * PIT_SYSTIME_COUNT is decremented by one each time the PIT's internal clock
 * oscillates.
 *
 * PIT_SYSTIME_PERIOD is the amount of time it takes (in microseconds) for the
 * PIT to decrement its count to 0. PIT_SYSTIME_PERIOD should use the
 * preprocessor do the math, but I did the math manually here...
 */
#define PIT_SYSTIME_PERIOD 54912 /* in microseconds */
#define PIT_SYSTIME_COUNT 65520

/* I/O ports */
#define PIT_COUNTER_0_REG 0x40
#define PIT_COUNTER_1_REG 0x41
#define PIT_COUNTER_2_REG 0x42
#define PIT_CTRL_WORD_REG 0x43

/* Control Word format:
 *
 * Bit 0: count mode (0/1) -> (binary / binary coded decimal)
 *
 * Bit 1-3: operating mode
 * 000: interrupt on terminal count
 * 001: programmable one-shot
 * 010: rate generator
 * 011: square wave generator
 * 100: software triggered strobe
 * 101: hardware triggered strobe
 * 110: undefined (alias for mode 2)
 * 111: undefined (alias for mode 3)
 *
 * Bit 4-5: read/load mode
 * 00: counter value -> internal control register at time of I/O write
 * 01: least significant byte only
 * 10: most significant byte only
 * 11: least followed by most significant byte
 *
 * Bit 6-7: select counter
 * 00: counter 0
 * 01: counter 1
 * 10: counter 2
 * 11: undefined
 */

#define PIT_COUNT_MODE_BIN 0x00
#define PIT_COUNT_MODE_BCD 0x01

#define PIT_OPERATING_MODE_INT_ON_TERM_CNT 0x00 /* 0bxxxx000x */
#define PIT_OPERATING_MODE_ONE_SHOT        0x02 /* 0bxxxx001x */
#define PIT_OPERATING_MODE_RATE_GEN        0x04 /* 0bxxxx010x */
#define PIT_OPERATING_MODE_SQUARE_WAVE     0x06 /* 0bxxxx011x */
#define PIT_OPERATING_MODE_SW_STROBE       0x08 /* 0bxxxx100x */
#define PIT_OPERATING_MODE_HW_STROBE       0x0A /* 0bxxxx101x */

#define PIT_READ_LOAD_MODE_LATCH   0x00 /* 0bxx00xxxx */
#define PIT_READ_LOAD_MODE_LSB     0x10 /* 0bxx01xxxx */
#define PIT_READ_LOAD_MODE_MSB     0x20 /* 0bxx10xxxx */
#define PIT_READ_LOAD_MODE_LSB_MSB 0x30 /* 0bxx11xxxx */

#define PIT_SELECT_COUNTER_0 0x00	/* 0b00xxxxxx */
#define PIT_SELECT_COUNTER_1 0x40	/* 0b01xxxxxx */
#define PIT_SELECT_COUNTER_2 0x80	/* 0b10xxxxxx */

#define PIT_INIT_IRQ0 PIT_COUNT_MODE_BIN | PIT_OPERATING_MODE_SQUARE_WAVE | PIT_READ_LOAD_MODE_LSB_MSB | PIT_SELECT_COUNTER_0


void pit_send_cmd (uint8_t cmd);

/* Minimum Hz: ~18.2 Hz; Maximum Hz: ~0.596 MHz */
void pit_send_count (int counter_port, int cycles);


#endif	/* PIT_8253_H */
