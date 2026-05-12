#ifndef TIMER_H_
#define TIMER_H_

#include "Std_Types.h"

/* Initializes TIM3 with the specified prescaler and auto-reload for 500ms */
void Timer3_Init(void);

/* Sets the callback to be executed inside the TIM3 ISR */
void Timer3_SetCallback(void (*Callback)(void));

#endif /* TIMER_H_ */
