/* File: kernel.c
 *
 * Entry point for the kernel
 */

#include <stdint.h>
#include "asm.h"
#include "idt.h"
#include "exceptions.h"
#include "vga_text_mode.h"
#include "pic.h"
#include "pit.h"
#include "time.h"
#include "systime.h"
#include "lib/list.h"
#include "drivers/ps2.h"

void print_text_clock ();
void update_kernel_runtime ();
void init_interrupt_controller ();
void init_interrupt_table ();
void init_system_timer ();
void init_ps2_controller ();

static struct time_t ktime; /* Kernel Runtime: leave initialized at 0 */

static struct timed_service ktime_service
= {.service = update_kernel_runtime,
   .flags = TIMER_RUNNING_MASK | TIMER_ROUTINE_MASK,
   .max_irq_wait = 0};

static struct timed_service text_clock_service
= {.service = print_text_clock,
   .flags = TIMER_RUNNING_MASK | TIMER_ROUTINE_MASK,
   .max_irq_wait = 0};


void print_text_clock ()
{
  char clock[] = "00:00:00:00";

  int hours   = ktime.time_sec / 3600;
  int minutes = (ktime.time_sec - (hours * 3600)) / 60;
  int seconds = ktime.time_sec - (minutes * 60) - (hours * 3600);

  clock[0] = hours / 10;
  clock[1] = hours - clock[0] * 10;
  clock[0] += '0';
  clock[1] += '0';

  clock[3] = minutes / 10;
  clock[4] = minutes - clock[3] * 10;
  clock[3] += '0';
  clock[4] += '0';

  clock[6] = seconds / 10;
  clock[7] = seconds - clock[6] * 10;
  clock[6] += '0';
  clock[7] += '0';

  clock[9]  = ktime.time_usec / 100000;
  clock[10] = (ktime.time_usec - clock[9] * 100000) / 10000;
  clock[9]  += '0';
  clock[10] += '0';

  write_str(clock, 11, 34, 11);
}

/* Update elapsed time */
void update_kernel_runtime ()
{
  ktime.time_usec += PIT_SYSTIME_PERIOD;

  if (ktime.time_usec >= 1000000)
    {
      ktime.time_sec++;
      ktime.time_usec -= 1000000;
    }
}

/* Setup 8259 PIC */
inline void init_interrupt_controller ()
{
  send_icw1();
  send_icw2();
  send_icw3();
  send_icw4();
  pic_disable_irqs(0xffff);
}

inline void init_interrupt_table ()
{
  idt_add_exceptions();
  idt_install_interrupt(0x20, &system_timer_interrupt, 0x8e); /* IRQ 0 */
  idt_load();
}

inline void init_system_timer ()
{
  systimer_add(&ktime_service);
  systimer_add(&text_clock_service);

  pit_send_cmd(PIT_INIT_IRQ0);
  pit_send_count(PIT_COUNTER_0_REG, PIT_SYSTIME_COUNT);
}

void init_ps2_controller ()
{
  char* ps2_msg = "PS/2 controller detected";
  char* init_msg = "PS/2: initializing";

  char* msg1 = "PS/2: controller enabled";
  char* msg2 = "PS/2: initialization failed";

  write_str(ps2_msg, strlen(ps2_msg), 0, 0);
  write_str(init_msg, strlen(init_msg), 0, 1);

  int success = ps2_initialize();

  if (success)			/* config_byte_disable()? */
    write_str(msg1, strlen(msg1), 0, 2);
  else
    write_str(msg2, strlen(msg2), 0, 2);
}

void hello_world ()
{
  char* msg = "Welcome to the Botnet";
  char* meme = ":^)";

  set_gbl_text_attr(0x30);	/* bg: teal, fg: black  */
  clear_screen();

  /* write_str(msg, 21, 30, 11);
  write_str(meme, 3, 0, 0);
  write_str(meme, 3, 77, 0);
  write_str(meme, 3, 0, 24);
  write_str(meme, 3, 77, 24); */
}

int main (uint32_t* multiboot_struct) /* replace uint32_t */
{
  // this is not currently used
  // this would be for supporting multiboot
  // return if not multiboot
  asm("cmp $0x2badb002, %eax\n"	/* if (IS_MULTIBOOT_COMPATIBLE) */
      "je is_multiboot\n"
      "ret\n"
      "is_multiboot:\n");

  hello_world();

  init_interrupt_table();
  init_interrupt_controller();
  init_system_timer();

  pic_disable_irqs(0xfffe);	/* enable IRQ 0 */

  asm("sti");

  init_ps2_controller();

  while (1)
    {
      asm("hlt");
    }


  /* intentional divide by 0 for testing when while loop is commented out */
  int k = 4;
  k /= 0;
}
