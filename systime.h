/* File: systime.h
 *
 * Timed service struct, function declarations, and inline functions for
 * checking and modifying timed service struct flags.
 */

#include <stdint.h>
#include "pit.h"

#ifndef SYSTIME_H
#define SYSTIME_H

#define TIMER_RUNNING_MASK 0x01
// looks like the routine bit is not currently used
#define TIMER_ROUTINE_MASK 0x02
// one shot is for a one-time timer, it would not be reset on expiration
// not currently checked though in systimer_check()
#define TIMER_ONE_SHOT_MASK 0x04

#define is_timer_expired(timer) (timer->rem_irq_wait <= 0)

struct timed_service
{
  void (*service)(void);
  uint8_t flags;
  int max_irq_wait;
  int rem_irq_wait;
};

extern uint32_t irq0_count;


int irq_spin_wait (int (*condition)(void), int wait);
struct time_t systime_get_time_t ();
void systimer_add (struct timed_service* timer);
void system_timer_interrupt ();

static inline void timer_start (struct timed_service* timer)
{
  // set TIMER_RUNNING_MASK bit
  timer->flags |= 0x01;		/* set bit 0 */
}

static inline void timer_stop (struct timed_service* timer)
{
  timer->flags &= 0xfe;		/* clear bit 0 */
}

static inline void timer_reset (struct timed_service* timer)
{
  timer_stop(timer);
  timer->rem_irq_wait = timer->max_irq_wait;
}

static inline void timer_enable_routine (struct timed_service* timer)
{
  timer->flags |= 0x02;		/* set bit 1 */
}

static inline void timer_disable_routine (struct timed_service* timer)
{
  timer->flags &= 0xfd;		/* clear bit 1 */
}

static inline void timer_set_one_shot (struct timed_service* timer)
{
  timer->flags |= 0x04;		/* set bit 2 */
}

static inline void timer_set_continuous (struct timed_service* timer)
{
  timer->flags &= 0xfb;		/* clear bit 2 */
}

static inline int is_timer_running (struct timed_service* timer)
{
  return timer->flags & TIMER_RUNNING_MASK;
}

static inline int is_timer_routine (struct timed_service* timer)
{
  return timer->flags & TIMER_ROUTINE_MASK && timer->service;
}

static inline int is_timer_one_shot (struct timed_service* timer)
{
  return timer->flags & TIMER_ONE_SHOT_MASK;
}


#endif	/* SYSTIME_H */
