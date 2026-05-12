#include "Timer.h"
#include "Mcu_Hw.h"
#include "Bit_Operations.h"
#include "Nvic.h"

volatile uint8 g_tick_500ms = 0U;

void Timer_Init_TIM3_500ms(uint32 sys_clock_freq) {
    /* Enable TIM3 Clock (APB1, Bit 1) */
    SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM3EN_Pos);
    
    /* Calculate Prescaler for 10KHz tick (0.1ms) */
    /* Assuming APB1 timer clock is sys_clock_freq */
    TIM3->PSC = (sys_clock_freq / 10000U) - 1U;
    
    /* Set Auto-Reload Register for exactly 500ms (5000 * 0.1ms = 500ms) */
    TIM3->ARR = 5000U - 1U;
    
    /* Enable Update Interrupt */
    SET_BIT(TIM3->DIER, 0U); /* UIE */
    
    /* Enable TIM3 in NVIC */
    Nvic_EnableIrq(TIM3_IRQn);
    Nvic_SetPriority(TIM3_IRQn, 3U, 0U);
    
    /* Enable TIM3 */
    SET_BIT(TIM3->CR1, TIM_CR1_CEN_Pos);
}

void TIM3_IRQHandler(void) {
    /* Check update interrupt flag */
    if (GET_BIT(TIM3->SR, 0U)) {
        /* Clear update interrupt flag */
        CLR_BIT(TIM3->SR, 0U);
        
        /* Set 500ms tick flag to defer processing to the main loop */
        g_tick_500ms = 1U;
    }
}
