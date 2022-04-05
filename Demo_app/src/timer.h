#ifndef TIMER_H
#define TIMER_H

#include "scheduler.h"

//Timer related:
#define  TMR1Reload      2000     //running @ 16MIPS

cntr_size_t get_ticks_counter(void);

#endif
