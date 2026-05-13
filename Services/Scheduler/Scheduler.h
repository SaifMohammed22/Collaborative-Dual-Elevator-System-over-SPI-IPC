#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include "Std_Types.h"

/* Initializes a 50ms periodic scheduler using TIM2. */
void Scheduler_Init_50ms(uint32 sys_clock_freq);

/* Flag set by TIM2 ISR when 50ms tick occurs. Poll from main loop. */
extern volatile uint8 g_tick_50ms;

#endif /* SCHEDULER_H_ */
