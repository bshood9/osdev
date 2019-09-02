/* File: systime.c
 *
 * IRQ spin wait
 * Maintains the list of timers
 * PIT interrupt handler
 */
#include "systime.h"

/* increasing this may require an update to node_allocation_bitmap */
/* because it currently uses a 16 bit type, uint16_t */
#define PREALLOCATED_LIST_HEADS 16 /* TODO: dynamic allocation */

static struct list_head preallocated_nodes[PREALLOCATED_LIST_HEADS] = {0};
/* the type of node_allocation_bitmap needs to be large enough to hold the */
/* number of pre-allocated nodes (PREALLOCATED_LIST_HEADS) */
static uint16_t node_allocation_bitmap = 0;
static struct list_head* running_list = NULL;

/* global to this file, initialized to 0 */
/* incremented in system_timer_interrupt() */
uint32_t irq0_count = 0;


/* spin until 'wait' number of interrupts have transpired or
 * condition() returns zero
 * return amount of 'wait' remaining
 */
int irq_spin_wait (int (*condition)(void), int wait)
{
  uint32_t wait_irq = irq0_count + wait;

  while (wait && condition())
    {
	  // not concerned about irq0_count overflow as that would take
	  // far longer than we care about
      wait = wait_irq - irq0_count;
    }

  return wait; 
}


 /*
int irq_spin_wait (int (*condition)(void), int wait)
{
  while (wait--)
    {
	  // approach here was to halt the CPU, which would
	  // resume upon encountering an interrupt, which is expected to
	  // be the timer interrupt
	  //
	  // however, other interrupts would also cause the CPU to resume,
	  // so would need to do something such as check the irq0_count value
	  // to determine whether to re-halt or return
      asm("hlt");
    }

  // in this version the condition is only checked after wait expiration
  // this could be relocated inside the while loop if this version were used
  return !condition();
}
 */

/* this function is not currently used.
 * this is getting the number of microseconds that have elapsed since
 * startup. it uses assembly to divide to get seconds and microseconds.
 *
 * irq0_count * PIT_SYSTIME_PERIOD / 1,000,000 is the equation.
 * doing the division in assembly is faster than in C.
 */
struct time_t systime_get_time_t ()
{
  struct time_t time = {0, 0};
  uint32_t seconds;
  uint32_t milliseconds;

  asm volatile("movl %2, %%eax\n\t"
	       "movl %3, %%edx\n\t"
	       "mul %%edx\n\t"
	       "movl $1000000, %%ecx\n\t"
	       "div %%ecx\n\t"
	       "movl %%eax, %0\n\t"
	       "movl %%edx, %1\n\t"

	       : "=rm" (time.time_sec), "=rm" (time.time_usec)
	       : "rm" (irq0_count), "rm" (PIT_SYSTIME_PERIOD)
	       : "cc", "eax", "edx", "ecx");

  return time;
}

static struct list_head* list_head_alloc ()
{
  int i;

  for (i = 0; i < PREALLOCATED_LIST_HEADS; ++i)
    {
      if ((node_allocation_bitmap & (1 << i)) == 0)
	{
	  node_allocation_bitmap |= (1 << i);

	  return &preallocated_nodes[i];
	}
    }

  return NULL;
}

static void list_head_free (struct list_head* node)
{
  int node_index;

  node_index = (node - preallocated_nodes) / sizeof(struct list_head);
  node_allocation_bitmap &= ~(1 << node_index);
  // the data is not cleared here, perhaps that would be good to add
}

// check whether CPU interrupt flag is set
// return whether the interrupt flag was set
// the CPU interrupt flag tells whether interrupts are currently enabled
static inline int check_interrupt_flag ()
{
  int iflag;

  // push flags register onto stack
  // 0x0200 is the bit to inspect for enabled interrupts
  asm volatile("pushf\n\t"
	       "pop %%eax\n\t"
	       "mov $0x0200, %%dx\n\t"
	       "test %%ax, %%dx\n\t"
	       "setnz %%al\n\t"
	       "movzx %%al, %%eax\n\t"
	       "mov %%eax, %0"

	       : "=rm" (iflag)	/* output operands */
	       :		/* input operands */
	       : "cc", "eax");	/* clobbers */

  return iflag;
}

void systimer_add (struct timed_service* timer)
{
  // should rename from 'head'
  struct list_head* new_node = list_head_alloc();

  if (new_node)
    {
      new_node->value = timer;
      timer->rem_irq_wait = timer->max_irq_wait + 1; /* ignore first irq */
	  // start the timer
	  // the 2 timers currently used run continually to implement a particular
	  // period
	  // since the list is traversed each time the interrupt is handled,
	  // implementing an occassional timer such as a sleep() may be better
	  // to allocate anew on each use rather than keeping in the list
      timer_start(timer);

      // if interrupts are enabled, disable then re-enable while we
      // update the list to prevent concurrency problems
      // any interrupt received during that time will have to wait until
      // interrupts are reenabled before it can be serviced
      if (check_interrupt_flag())
	{
	  asm("cli");
	  list_append(&running_list, new_node);
	  asm("sti"); 
	}
      else
	list_append(&running_list, new_node);
    }
}

static void systimer_remove (struct list_head* node)
{
  // removal should also check and possibly unset the interrupt flag
  list_remove(&running_list, node);
  list_head_free(node);
}

static void systimer_check (struct list_head* node)
{
  struct timed_service* timer = node->value;

  if (is_timer_running(timer))
    {
      --(timer->rem_irq_wait);

      if (is_timer_expired(timer))
	{
	  timer->service();
	  timer->rem_irq_wait = timer->max_irq_wait;
	}
    }
  else
    systimer_remove(node);
}

static inline void update_running_list ()
{
  struct list_head* tmp = running_list;

  // return if running_list is NULL
  while (tmp)
    {
      systimer_check(tmp);
      tmp = tmp->next;

	  // back at beginning of circular list
      if (tmp == running_list)
	break;	
    }
}

// interrupt handler for programmable interval timer
void system_timer_interrupt ()
{
  //asm("xchg %bx, %bx");

  ++irq0_count;
  update_running_list();
  // tells the PIC that we are done servicing the interrupt so that
  // if there is another pending it can now be provided
  outb(PIC_MASTER_CMD_REG, 0x20); /* send EOI */

  // see exceptions.c comments for explanation as to why we do return far
  asm("leave");
  asm(".byte 0xcb");		/* retf */
}

/*
  if (is_timer_routine(timer))
  timer->service();

  if (is_timer_one_shot(timer))
  {
  timer_stop(timer);
  pit_remove_timer_list(node);
  }
*/

/*
static inline void start_waiting_list ()
{
  if (waiting_list)
    {
      list_for_all(&waiting_list, &timer_start);
      list_concat(&running_list, &waiting_list);
      waiting_list = NULL;
    }
}
*/
