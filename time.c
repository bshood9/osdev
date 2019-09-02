/* File: time.c
 *
 * Functions for checking and modifying the timed service struct flags
 * Replaced by inline functions in systime.c
 */

#include "time.h"


void timer_start (struct timed_service* timer)
{
  timer->flags |= 0x01;
}

void timer_stop (struct timed_service* timer)
{
  timer->flags &= 0xfe;
}

void timer_reset (struct timed_service* timer)
{
  timer_stop(timer);
  timer->rem_irq_wait  = timer->max_irq_wait;
}

void timer_enable_routine (struct timed_service* timer)
{
  timer->flags |= 0x02;
}

void timer_disable_routine (struct timed_service* timer)
{
  timer->flags &= 0xfd;
}

void timer_set_one_shot (struct timed_service* timer)
{
  timer->flags |= 0x04;
}

void timer_set_continuous (struct timed_service* timer)
{
  timer->flags &= 0xfb;
}

int is_timer_running (struct timed_service* timer)
{
  return timer->flags & TIMER_RUNNING_MASK;
}

int is_timer_routine (struct timed_service* timer)
{
  return timer->flags & TIMER_ROUTINE_MASK && timer->service;
}

int is_timer_one_shot (struct timed_service* timer)
{
  return timer->flags & TIMER_ONE_SHOT_MASK;
}
