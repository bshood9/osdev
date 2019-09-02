/* File: ps2.h
 *
 * PS/2 controller info and port status structs, macros, and function prototypes
 */

#include <stdint.h>
#include "../systime.h"
#include "../idt.h"
#include "../io.h"
#include "../pit.h"
#include "keyboard.h"

#ifndef PS2_H
#define PS2_H

/* I/O Ports */
#define PS2_DATA_PORT  0x60
#define PS2_STATUS_REG 0x64	/* accessed on read */
#define PS2_CMD_REG    0x64	/* accessed on write */

/* PS/2 controller status register bitmasks */
#define OUTPUT_BUFFER_STATUS_MASK 0x01
#define INPUT_BUFFER_STATUS_MASK  0x02
#define SYSTEM_FLAG_MASK          0x04
#define COMMAND_DATA_MASK         0x08
#define TIMEOUT_ERROR_MASK        0x40
#define PARITY_ERROR_MASK         0x80

/* PS/2 controller command bytes */
#define PS2_READ_CONFIG_BYTE  0x20
#define PS2_WRITE_CONFIG_BYTE 0x60

#define PS2_TEST_CONTROLLER 0xaa
#define PS2_DIAGNOSTIC_DUMP 0xac 

#define PS2_TEST_PORT1    0xab
#define PS2_ENABLE_PORT1  0xae
#define PS2_DISABLE_PORT1 0xad

#define PS2_TEST_PORT2    0xa9
#define PS2_ENABLE_PORT2  0xa8
#define PS2_DISABLE_PORT2 0xa7

#define PS2_READ_CONTROLLER_INPUT_PORT 0xc0
#define PS2_INPUT_PORT_LOW_TO_STATUS   0xc1
#define PS2_INPUT_PORT_HIGH_TO_STATUS  0xc2

#define PS2_READ_CONTROLLER_OUTPUT_PORT  0xd0
#define PS2_WRITE_CONTROLLER_OUTPUT_PORT 0xd1
#define PS2_PULSE_CPU_RESET_LINE         0xfe

#define PS2_WRITE_PORT1_OUTPUT_BUFFER 0xd2
#define PS2_WRITE_PORT2_OUTPUT_BUFFER 0xd3
#define PS2_WRITE_PORT2_INPUT_BUFFER  0xd4

/* PS/2 controller configuration-byte bitmasks */
#define PORT1_INTERRUPT      0x01	/* 0 = disabled, 1 = enabled */
#define PORT2_INTERRUPT      0x02	/* 0 = disabled, 1 = enabled */
#define SYSTEM_FLAG          0x04	/* 0 = POST failed, 1 = POST passed */
#define PORT1_CLOCK_DISABLED 0x10	/* 0 = enabled, 1 = disabled */
#define PORT2_CLOCK_DISABLED 0x20	/* 0 = enabled, 1 = disabled */
#define PORT1_TRANSLATION    0x40	/* 0 = disabled, 1 = enabled */

/* PS/2 device command bytes */
#define PS2_DEVICE_ECHO             0xee /* mouse responds differently */
#define PS2_DEVICE_IDENTIFY         0xf2
#define PS2_DEVICE_AUTO_REPEAT_RATE 0xf3
#define PS2_DEVICE_ENABLE_SCANNING  0xf4
#define PS2_DEVICE_DISABLE_SCANNING 0xf5
#define PS2_DEVICE_SET_DEFAULT      0xf6
#define PS2_DEVICE_RESEND           0xfe
#define PS2_DEVICE_RESET            0xff

/* PS/2 device response bytes */
#define PS2_DEVICE_BAT_PASSED 0xaa
#define PS2_DEVICE_BAT_FAILED 0xfc /* also 0xfd */
#define PS2_DEVICE_RESEND     0xfe
#define PS2_DEVICE_ACK        0xfa

/* PS/2 device types */
#define NO_DEVICE        0xff
#define AT_KEYBOARD      0x01	/* none */
#define MOUSE_STANDARD   0x00
#define MOUSE_WITH_WHEEL 0x03
#define MOUSE_5_BUTTON   0x04
#define MF2_KEYBOARD_WITH_TRANSLATION 0x41
#define MF2_KEYBOARD     0xab

/* PS/2 driver errno masks */
#define OUTPUT_BUFFER_TIMEOUT 0x01
#define INPUT_BUFFER_TIMEOUT  0x02
#define DEVICE_NOT_DETECTED   0x04
#define DEVICE_BAT_FAILURE    0x08
#define DEVICE_RESPONSE_NACK  0x10

#define PS2_NOT_SUPPORTED     0x20
#define PORT2_NOT_SUPPORTED   0x40
#define PORT1_INTERFACE_ERROR 0x80
#define PORT2_INTERFACE_ERROR 0x100
#define PS2_INIT_FAILURE      0x200

struct ps2_controller_info	/* make everything a bit flag -> uint8_t?? */
{
  int ps2_exists;		/* flag: PS/2 controller is present */
  int ps2_status;		/* flag: PS/2 controller is usable */
  int port1_status;		/* flag: port 1 is usable */
  int port2_status;		/* flag: port 2 is usable */
  int port1_device;		/* port 1 device type */
  int port2_device;		/* port 2 device type */
};

struct port_status
{
  int interface_status;
  int device_status;
  int device_type;
};

int input_buffer_full ();
int output_buffer_empty ();

int ps2_send_cmd (uint8_t cmd);
int ps2_send_data (uint8_t byte);
int ps2_read_data (uint8_t* buf);

int ps2_get_config_byte (uint8_t* buf);
int ps2_config_byte_enable (uint8_t mask);
int ps2_config_byte_disable (uint8_t mask);

int port1_send_cmd (uint8_t cmd);
int port2_send_cmd (uint8_t cmd);

int port1_reset_device ();
int port2_reset_device ();

int port1_detect_device ();
int port2_detect_device ();

int port1_echo_device ();
int port2_echo_device ();

void ps2_port1_plug_n_play ();
void ps2_port2_plug_n_play ();

void ps2_cpu_reset ();

int ps2_initialize ();


#endif	/* PS2_H */
