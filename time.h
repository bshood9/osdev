/* File: time.h
 *
 * Time struct
 */

#include <stdint.h>

#ifndef TIME_H
#define TIME_H

struct time_t
{
  uint32_t time_sec;		/* good for ~136 years */
  uint32_t time_usec;
};


#endif	/* TIME_H */
