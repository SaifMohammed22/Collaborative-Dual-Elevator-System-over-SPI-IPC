#ifndef TIMER_H_
#define TIMER_H_

#include "Std_Types.h"

/* Global flag for 500ms tick */
extern volatile uint8 g_tick_500ms;

/* Initializes TIM3 with the specified prescaler and auto-reload for 500ms */
void Timer_Init_TIM3_500ms(uint32 sys_clock_freq);

#endif /* TIMER_H_ */
