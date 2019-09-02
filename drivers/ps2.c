/* File: ps2.c
 *
 * PS2 Driver
 */

#include "ps2.h"

/* return 1 if the input function returns 0 and the wait did not expire
 * return 0 if the wait is reached before the input function returns 0
 *
 * wait is the number of PIT interrupts to wait
 * note that irq_spin_wait will spin in a loop until wait expires
 */

/* return >0 if input buffer is empty
 * return 0 if wait expired and the input buffer is still not empty
 */
#define wait_input_buffer(wait) (irq_spin_wait(&input_buffer_full, wait))
/* return >0 if output buffer is not empty
 * return 0 if wait expired and the output buffer is still empty
 */
#define wait_output_buffer(wait) (irq_spin_wait(&output_buffer_empty, wait))

static struct timed_service port1_pnp
= {ps2_port1_plug_n_play, TIMER_ROUTINE_MASK, 40, 0};

static struct timed_service port2_pnp
= {ps2_port2_plug_n_play, TIMER_ROUTINE_MASK, 40, 0};

static struct port_status port1_info;
static struct port_status port2_info;

/* return 1 if input buffer is full, 0 if empty
 * (returns whether the PS2 status register bit 1 is set)
 */
int input_buffer_full ()
{
  // bit 1 of the status register is the input buffer status
  // 0=empty, 1=full
  // 0x02 is the first bit, or bit 1
  return inb(PS2_STATUS_REG) & 0x02;
}

/* return 1 if output buffer is empty, otherwise 0
 * (returns whether the PS2 status register bit 0 is set)
 */
int output_buffer_empty ()
{
  // bit 0 of the status register is the output buffer status
  // 0=empty, 1=full
  // 0x01 is the first bit, or bit 0
  return !(inb(PS2_STATUS_REG) & 0x01);
}

/* send command to PS/2 controller by writing to the command register
 * return whether it was successful
 */
int ps2_send_cmd (uint8_t cmd)
{
  // wait for input buffer to be empty
  int wait_success = wait_input_buffer(2);

  // if empty then send the command
  if (wait_success)
    outb(PS2_CMD_REG, cmd);

  return wait_success;
}

/* send data to PS/2 controller by writing 'byte' to the data register
 * return whether it was successful
 */
int ps2_send_data (uint8_t byte)
{
  int wait_success = wait_input_buffer(2);

  if (wait_success)
    outb(PS2_DATA_PORT, byte);

  return wait_success;
}

/* read a byte from the data port */
int ps2_read_data (uint8_t* buf)
{
  int wait_success = wait_output_buffer(5);

  if (wait_success)
    *buf = inb(PS2_DATA_PORT);

  return wait_success;
}

/* read config byte from the data port */
int ps2_get_config_byte (uint8_t* buf)
{
  return ps2_send_cmd(PS2_READ_CONFIG_BYTE) && ps2_read_data(buf);
}

/* set bits in the PS/2 config byte */
int ps2_config_byte_enable (uint8_t mask)
{
  uint8_t buf;
  int ret = -1;

  if (ps2_get_config_byte(&buf))
    {
      buf |= mask;

      if (ps2_send_cmd(PS2_WRITE_CONFIG_BYTE) && ps2_send_data(buf))
	ret = buf;
    }

  return ret; /* returns -1 on error, otherwise the new config byte */
}

/* clear bits in the PS/2 config byte */
int ps2_config_byte_disable (uint8_t mask)
{
  uint8_t buf;
  int ret = -1;

  if (ps2_get_config_byte(&buf))
    {
      buf &= ~mask;
      
      if (ps2_send_cmd(PS2_WRITE_CONFIG_BYTE) && ps2_send_data(buf))
	ret = buf;
    }

  return ret; /* returns -1 on error, otherwise the new config byte */
}

/* send a command to the PS/2 controller and compare the PS/2 controller's
 * response with the expected value
 */
static int ps2_check_response (uint8_t cmd, uint8_t expected)
{
  uint8_t buf;
  int ret = -1;

  if (ps2_send_cmd(cmd) && ps2_read_data(&buf))
    ret = buf == expected;

  return ret; /* returns -1 on error, otherwise the result of comparison */
}

/* send a command to the device attached to the PS/2 controller
 * and compare the device's response with the expected value
 */
static int device_check_response (uint8_t cmd, uint8_t expected)
{
  uint8_t buf;
  int ret = -1;

  if (ps2_send_data(cmd) && ps2_read_data(&buf))
    ret = buf == expected;
  
  return ret; /* returns -1 on error, otherwise the result of comparison */
}

/* check whether the specified bits in the config byte are set */
static int test_config_byte (uint8_t mask)
{
  uint8_t buf;
  int ret = -1;

  if (ps2_get_config_byte(&buf))
    ret = buf & mask;

  return ret; /* returns -1 on error, otherwise the result of "buf & mask" */
}

/* check whether the PS/2 controller is dual channel (has two device ports)
 * return 1 if dual channel, 0 if not, -1 on error
 * port2 clock should be disabled at the start of this function
 * if the port2 clock is enabled, then port2 is broken or doesn't exist
 * if the command to enable port2 does not enable the port2 clock,
 * then the PS/2 controller not dual channel
 */
static inline int is_dual_channel ()
{
  int ret = -1;

  switch (test_config_byte(PORT2_CLOCK_DISABLED))
    {
    case 1:
      {
	if (ps2_send_cmd(PS2_ENABLE_PORT2))
	  {
	    switch (test_config_byte(PORT2_CLOCK_DISABLED))
	      {
	      case 0:
		{
		  if (ps2_send_cmd(PS2_DISABLE_PORT2))
		    ret = 1;

		  break;
		}

	      case 1:
		ret = 0;

	      case -1:
		;
	      }
	  }
	
	break;
      }

    case 0:
      ret = 0;

    case -1:
      ;
    }

  return ret;	  /* returns -1 on error, otherwise whether port2 exists */
}

/* check if the PS/2 controller is dual channel, and if it is
 * then send the test port2 command and verify that it passed the test
 */
static inline int ps2_test_port2 ()
{
  return (is_dual_channel() == 1 &&
	  ps2_check_response(PS2_TEST_PORT2, 0x00) == 1);
}

/* send the test port1 command and verify that it passed the test */
static inline int ps2_test_port1 ()
{
  return ps2_check_response(PS2_TEST_PORT1, 0x00) == 1;
}

/* send the input command to the device on port1 and verify that the
 * device acknowledged it
 */
int port1_send_cmd (uint8_t cmd)
{
  uint8_t buf;

  return (ps2_send_data(cmd) &&
	  ps2_read_data(&buf) &&
	  buf == PS2_DEVICE_ACK);
}

/* tell PS/2 controller that the command is for the second device, and
 * send the input command to the device on port2 and verify that the
 * device acknowledged it
 */
int port2_send_cmd (uint8_t cmd)
{
  return (ps2_send_cmd(PS2_WRITE_PORT2_INPUT_BUFFER) &&
	  port1_send_cmd(cmd));
}

/* send the device reset command to the device on port1 and verify that the
 * device specific BAT passed
 */
int port1_reset_device ()
{
  uint8_t buf;
  /* prior implementation; turned out that 12 wait periods was not necessary
  if (port1_info.interface_status)
    {
      if (port1_send_cmd(PS2_DEVICE_RESET))
	{
	  if (wait_output_buffer(12))
	    {
	      buf = inb(PS2_DATA_PORT);

	      if (buf == PS2_DEVICE_BAT_PASSED)
		return 1;
	    }
	}
    }

  return 0;
  */
  
  return (port1_info.interface_status &&
	  port1_send_cmd(PS2_DEVICE_RESET) && /* byte 0xfa */
	  ps2_read_data(&buf) &&
  	  buf == PS2_DEVICE_BAT_PASSED); /* byte 0xaa */
}

/* send the device reset command to the device on port2 and verify that the
 * device specific BAT passed
 */
int port2_reset_device ()
{
  uint8_t buf;

  return (port2_info.interface_status &&
	  port2_send_cmd(PS2_DEVICE_RESET) &&
	  ps2_read_data(&buf) &&
	  buf == PS2_DEVICE_BAT_PASSED);
}

/* determines whether connected device is MF2 KEYBOARD,
 * MF2 KEYBAORD with translation, or AT KEYBOARD
 * 0x83 value should be added to ps2.h
 * could be extended to identify other device types
 * if an AT KEYBOARD then there is no byte to read
 */
static void identify_device (uint8_t* buf)
{
  if (ps2_read_data(buf))
    {
      if (*buf == MF2_KEYBOARD)
	{
	  if (ps2_read_data(buf))
	    {
	      if (*buf == 0x83)
		*buf = MF2_KEYBOARD;

	      else
		*buf = MF2_KEYBOARD_WITH_TRANSLATION;
	    }
	  // if we don't get a second byte here then it is an error
	  else
	    *buf = NO_DEVICE;
	}
    }
  else
    *buf = AT_KEYBOARD;
}

/* wrapper that disables scanning on port1, identifies the device, then
 * reenables scanning
 */
int port1_detect_device ()
{
  uint8_t buf = NO_DEVICE;

  /* if device has scanning enabled then we cannot identify the device
   * because key presses will overwrite what is in the input buffer
   * *possibly*
   * disable device before identificaiton
   */
  if (port1_info.device_status &&
      port1_send_cmd(PS2_DEVICE_DISABLE_SCANNING) &&
      port1_send_cmd(PS2_DEVICE_IDENTIFY))
    {
      // these are magic breakpoints for the bochs debugger
      //      asm("xchg %bx, %bx");
      identify_device(&buf);
      //      asm("xchg %bx, %bx");
    }

  // re-enable device
  if (buf != NO_DEVICE)
    port1_send_cmd(PS2_DEVICE_ENABLE_SCANNING);

  //  asm("xchg %bx, %bx");
  return buf;
}

/* wrapper that disables scanning on port2, identifies the device, then
 * reenables scanning
 */
int port2_detect_device ()
{
  uint8_t buf = NO_DEVICE;

  if (port2_info.device_status &&
      port2_send_cmd(PS2_DEVICE_DISABLE_SCANNING) &&
      port2_send_cmd(PS2_DEVICE_IDENTIFY))
    {
      identify_device(&buf);
    }

  if (buf != NO_DEVICE)
    port2_send_cmd(PS2_DEVICE_ENABLE_SCANNING);

  return buf;
}

/* echo device on port1 */
int port1_echo_device ()
{
  // device echo command puts the same device echo value in the port's data
  return device_check_response(PS2_DEVICE_ECHO, PS2_DEVICE_ECHO) == 1;
}

/* echo device on port2 */
int port2_echo_device ()
{
  // tell PS/2 that the input buffer is for port 2 then perform echo
  return (ps2_send_cmd(PS2_WRITE_PORT2_INPUT_BUFFER) &&
	  device_check_response(PS2_DEVICE_ECHO, PS2_DEVICE_ECHO) == 1);
}

/* install the interrupt handler associated with the device type on port1 */
static inline void port1_select_driver (uint8_t device_type)
{
  switch (device_type)
    {
    case AT_KEYBOARD:
      break;

    case MOUSE_STANDARD:
      break;

    case MOUSE_WITH_WHEEL:
      break;

    case MOUSE_5_BUTTON:
      break;

    case MF2_KEYBOARD_WITH_TRANSLATION:
      break;

    // aforementioned devices are not implemented
    case MF2_KEYBOARD:
      {
	// TODO: replace magic numbers
	idt_install_interrupt(0x21, &keyboard_interrupt, 0x8e);
	idt_load();
      }
      // TODO: add a case for NO_DEVICE
    }
}

/* install the interrupt handler associated with the device type on port2
 * port 2 does not allow MF2 keyboard with translation and AT keyboard
 */
static inline void port2_select_driver (uint8_t device_type)
{
  switch (device_type)
    {
    case MOUSE_STANDARD:
      break;

    case MOUSE_WITH_WHEEL:
      break;

    case MOUSE_5_BUTTON:
      break;

    case MF2_KEYBOARD:
      {
	idt_install_interrupt(0x2c, &keyboard_interrupt, 0x8e);
	idt_load();
      }
    }
}

/* set up plug n' play on port1 */
void ps2_port1_plug_n_play ()
{
  uint8_t device_type;

  //  asm("xchg %bx, %bx");

  if (port1_info.device_status)
    {
      // if device does not respond to an echo (no longer there)
      // then disable the interrupt for that device
      // this assumes that if a device is present it is the same device,
      // and that the device won't be swapped out for a different one within
      // the same polling period
      if (!(port1_info.device_status = port1_echo_device()))

      // disable the IRQ line for PS/2 port1 (set bit 1 of the mask)
      // this prevents the CPU from receiving spurious interrupts on PIC bit 1
	pic_disable_irqs(0xfffe);
    }
      
  // no device has been set up yet
  else
    {
      if (port1_reset_device())
	{
	  //	  asm("xchg %bx, %bx");
	  port1_info.device_status = 1;

	  device_type = port1_detect_device();

	  if (device_type == port1_info.device_type &&
	      device_type != NO_DEVICE)
	    {
	      // if it is NO_DEVICE, then we may have lost the config and
	      // may need to reset it

	      /* set up previously saved config */
	    }

	  // different device than before or NO_DEVICE
	  else
	    // should have some different handling for NO_DEVICE case
	    {
	      port1_info.device_type = device_type;
	      port1_select_driver(device_type);
	      // enable IRQ line 1 for PS/2 in the PIC
	      // TODO: this should not be done for NO_DEVICE
              // TODO: this bitmask may be wrong
	      pic_disable_irqs(0xfffd);
	    }
	}
    }
}

/* set up plug n' play on port2 */
void ps2_port2_plug_n_play ()
{
  uint8_t device_type;

  if (port2_info.device_status)
    {
      if (!(port2_info.device_status = port2_echo_device()))
	// TODO: should this be a different bit than for the port1 case?
	pic_disable_irqs(0xfffe);
    }

  else
    {
      if (port2_reset_device())
	{
	  port2_info.device_status = 1;

	  device_type = port2_detect_device();

	  if (device_type == port2_info.device_type &&
	      device_type != NO_DEVICE)
	    {
	      /* set up previously saved config */
	    }

	  else
	    {
	      port2_info.device_type = device_type;
	      port2_select_driver(port2_info.device_type);
	      // TODO: this should not be done for NO_DEVICE
              // TODO: this bitmask may be wrong
              // TODO: should be a different bit than port1
	      pic_disable_irqs(0xfffd);
	    }
	}
    }
}

/* reset CPU */
void ps2_cpu_reset ()
{
  ps2_send_cmd(PS2_PULSE_CPU_RESET_LINE); /* should never return */
}

/* test PS/2 controller
 * returns 1 on test passed, 0 on test failed, and -1 on error
 */
static int ps2_test_controller ()
{
  uint8_t buf;
  int ret = -1;

  if (ps2_send_cmd(PS2_TEST_CONTROLLER) && wait_output_buffer(12))
    {
      buf = inb(PS2_DATA_PORT);
      // TODO: change magic number
      ret = buf == 0x55;
    }

  return ret;
}

/* attempt to flush the buffers */
static int flush_buffers ()
{
  /* read the output buffer until it remains empty */
  while (wait_output_buffer(2))
    inb(PS2_DATA_PORT);

  /* wait up to 2 timer periods for the input buffer to be empty
   * that's all we can do. this is just a best effort attempt.
   */
  return wait_input_buffer(2);
}

/**
 * set_ps2_mode() - Set PS/2 controller mode to PS/2 (MCA) interface
 *
 * return 1 if the test passed and the command and data were sent successfully
 *
 * Command 0xca: Read keyboard controller mode 
 * Command 0xcb: Write keyboard controller mode
 * These are deprecated commands, but some Super I/Os need them for setup.
 * A similar procedure can be found in coreboot/keyboard.c -- and I'm assuming
 * that the coreboot developers know what they're talking about.
 */
// this function may not have done anything and not been needed after all
static int set_ps2_mode ()
{
  return (ps2_test_controller() &&
	  ps2_send_cmd(0xcb) &&
	  ps2_send_data(0x01));
}

/* reset PS/2 controller
 * return 1 on success, 0 on failure
 */
static int ps2_reset ()
{
  // return value not inspected
  set_ps2_mode();

  if (!ps2_send_cmd(PS2_DISABLE_PORT1)) return 0;
  if (!ps2_send_cmd(PS2_DISABLE_PORT2)) return 0;

  if (!flush_buffers()) return 0;

  if (!ps2_config_byte_disable(PORT1_INTERRUPT |
			       PORT2_INTERRUPT | PORT1_TRANSLATION))
    return 0;

  return 1;
}

/* initialize PS/2 controller
 * return 1 on success, 0 on failure
 */
int ps2_initialize ()
{
  /* TODO: initialize USB controllers */
  /* TODO: determine if controller exists (ACPI tables) */

  if (!ps2_reset()) return 0;

  // this is called by ps2_reset() which calls set_ps2_mode()
  // perhaps ps2_test_controller() should be removed from set_ps2_mode()
  // if indeed it may reset the controller to the defaults
  if (!ps2_test_controller()) return 0;

  if (!ps2_reset()) return 0; /* test may reset controller to defaults */

  port2_info.interface_status = ps2_test_port2();
  port1_info.interface_status = ps2_test_port1();

  if (port1_info.interface_status) /* if port1 is usable */
    {
      if (!ps2_send_cmd(PS2_ENABLE_PORT1)) return 0;
      if (!ps2_config_byte_enable(PORT1_INTERRUPT)) return 0;

      systimer_add(&port1_pnp);
    }

  if (port2_info.interface_status) /* if port2 is usable */
    {
      if (!ps2_send_cmd(PS2_ENABLE_PORT2)) return 0;
      if (!ps2_config_byte_enable(PORT2_INTERRUPT)) return 0;

      systimer_add(&port2_pnp);
    }

  // if neither port is usable, this constitutes a failure to
  // initialize the PS/2 controller
  return port1_info.interface_status || port2_info.interface_status;
}
