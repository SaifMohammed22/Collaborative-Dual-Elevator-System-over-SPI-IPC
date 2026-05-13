#include "Scheduler.h"
#include "Mcu_Hw.h"
#include "Bit_Operations.h"
#include "Nvic.h"

volatile uint8 g_tick_50ms = 0U;

void Scheduler_Init_50ms(uint32 sys_clock_freq) {
    /* Enable TIM2 Clock */
    SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM2EN_Pos);

    /* Prescaler to 10kHz tick (0.1ms) */
    TIM2->PSC = (sys_clock_freq / 10000U) - 1U;
    /* ARR for 50ms => 500 * 0.1ms ticks */
    TIM2->ARR = 500U - 1U;

    /* Enable Update Interrupt */
    SET_BIT(TIM2->DIER, TIM_DIER_UIE_Pos);

    /* Enable TIM2 IRQ */
    Nvic_EnableIrq(TIM2_IRQn);
    Nvic_SetPriority(TIM2_IRQn, 3U, 0U);

    /* Start TIM2 */
    SET_BIT(TIM2->CR1, TIM_CR1_CEN_Pos);
}

void TIM2_IRQHandler(void) {
    if (GET_BIT(TIM2->SR, TIM_SR_UIF_Pos)) {
        CLR_BIT(TIM2->SR, TIM_SR_UIF_Pos);
        g_tick_50ms = 1U;
    }
}
