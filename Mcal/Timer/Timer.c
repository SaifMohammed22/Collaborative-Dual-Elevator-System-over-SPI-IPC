#include "Timer.h"
#include "Mcu_Hw.h"
#include "Bit_Operations.h"
#include "Nvic.h"

extern uint32 SystemCoreClock;

static void (*Timer3_Callback)(void) = NULL_PTR;

void Timer3_Init(void) {
    /* 1. Enable TIM3 Clock (APB1) */
    SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM3EN_Pos);
    
    /* 2. Calculate Prescaler for 1ms tick 
       Assuming APB1 timer clock is SystemCoreClock.
       (If SystemCoreClock = 16MHz, PSC = 15999. If 84MHz, PSC = 83999) */
    TIM3->PSC = (SystemCoreClock / 1000U) - 1U;
    
    /* 3. Set Auto-Reload Register for 500ms */
    TIM3->ARR = 500U - 1U;
    
    /* 4. Enable Update Interrupt */
    SET_BIT(TIM3->DIER, 0U); /* UIE (Update Interrupt Enable) is bit 0 */
    
    /* 5. Enable TIM3 in NVIC */
    Nvic_EnableIrq(TIM3_IRQn);
    Nvic_SetPriority(TIM3_IRQn, 3U, 0U); /* Priority 3: lower than emergency stop */
    
    /* 6. Enable TIM3 */
    SET_BIT(TIM3->CR1, TIM_CR1_CEN_Pos);
}

void Timer3_SetCallback(void (*Callback)(void)) {
    Timer3_Callback = Callback;
}

void TIM3_IRQHandler(void) {
    /* Check update interrupt flag */
    if (GET_BIT(TIM3->SR, 0U)) {
        /* Clear update interrupt flag */
        CLEAR_BIT(TIM3->SR, 0U);
        
        if (Timer3_Callback != NULL_PTR) {
            Timer3_Callback();
        }
    }
}
