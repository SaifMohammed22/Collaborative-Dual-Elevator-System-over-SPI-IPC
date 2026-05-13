#include "Std_Types.h"

#define TURN_OFF_SLAVE 1U  /* Set to 1 to turn off, 0 to enable */

volatile uint32 g_SysTick_Ms = 0U;

void SysTick_Handler(void) {
    g_SysTick_Ms++;
}

int main(void) {
    #if TURN_OFF_SLAVE
        /* Safe, empty loop */
        while(1) {
        }
    #endif
}