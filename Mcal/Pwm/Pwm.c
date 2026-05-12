/**
 * @file    Pwm.c
 * @brief   MCAL — PWM driver implementation.
 */

#include "Pwm.h"
#include "Bit_Operations.h"

/* ------------------------------------------------------------------ */
/*  Internal: enable the timer's APB clock                             */
/* ------------------------------------------------------------------ */
static void Pwm_EnableTimerClock(TIM_TypeDef *timer)
{
    if      (timer == TIM2) { SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM2EN_Pos); }
    else if (timer == TIM3) { SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM3EN_Pos); }
    else if (timer == TIM4) { SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM4EN_Pos); }
    else if (timer == TIM5) { SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM5EN_Pos); }
    /* Add more timers as needed */
}

/* ------------------------------------------------------------------ */
/*  Initialise PWM channel                                             */
/* ------------------------------------------------------------------ */
void Pwm_Init(const Pwm_CfgType *cfg)
{
    TIM_TypeDef     *tim = cfg->Timer;
    Pwm_ChannelType  ch  = cfg->Channel;

    Pwm_EnableTimerClock(tim);

    /* Base time-base ------------------------------------------------ */
    tim->PSC = cfg->Prescaler;
    tim->ARR = cfg->Period;
    tim->CNT = 0U;

    /* PWM Mode 1 (OC1M = 0b110) + Preload enable ------------------- */
    /*  CCMR1 handles CH1/CH2 ; CCMR2 handles CH3/CH4                 */
    switch (ch) {
        case PWM_CHANNEL_1:
            tim->CCMR1 &= ~(TIM_CCMR1_OC1M_Msk | TIM_CCMR1_OC1PE_Msk);
            tim->CCMR1 |=  (0x06U << TIM_CCMR1_OC1M_Pos)   /* PWM mode 1 */
                         |  (1U    << TIM_CCMR1_OC1PE_Pos);  /* preload    */
            tim->CCR1   = 0U;                                 /* duty = 0% */
            SET_BIT(tim->CCER, TIM_CCER_CC1E_Pos);           /* enable OC  */
            break;

        case PWM_CHANNEL_2:
            tim->CCMR1 &= ~(TIM_CCMR1_OC2M_Msk | TIM_CCMR1_OC2PE_Msk);
            tim->CCMR1 |=  (0x06U << TIM_CCMR1_OC2M_Pos)
                         |  (1U    << TIM_CCMR1_OC2PE_Pos);
            tim->CCR2   = 0U;
            SET_BIT(tim->CCER, TIM_CCER_CC2E_Pos);
            break;

        case PWM_CHANNEL_3:
            tim->CCMR2 &= ~(TIM_CCMR2_OC3M_Msk | TIM_CCMR2_OC3PE_Msk);
            tim->CCMR2 |=  (0x06U << TIM_CCMR2_OC3M_Pos)
                         |  (1U    << TIM_CCMR2_OC3PE_Pos);
            tim->CCR3   = 0U;
            SET_BIT(tim->CCER, TIM_CCER_CC3E_Pos);
            break;

        case PWM_CHANNEL_4:
            tim->CCMR2 &= ~(TIM_CCMR2_OC4M_Msk | TIM_CCMR2_OC4PE_Msk);
            tim->CCMR2 |=  (0x06U << TIM_CCMR2_OC4M_Pos)
                         |  (1U    << TIM_CCMR2_OC4PE_Pos);
            tim->CCR4   = 0U;
            SET_BIT(tim->CCER, TIM_CCER_CC4E_Pos);
            break;
    }

    /* Generate an update event to latch PSC/ARR, then start --------- */
    SET_BIT(tim->EGR, TIM_EGR_UG_Pos);
    SET_BIT(tim->CR1, TIM_CR1_ARPE_Pos);  /* ARR preload */
    SET_BIT(tim->CR1, TIM_CR1_CEN_Pos);   /* start counter */
}

/* ------------------------------------------------------------------ */
void Pwm_SetDuty(TIM_TypeDef *timer, Pwm_ChannelType channel, uint32 duty)
{
    switch (channel) {
        case PWM_CHANNEL_1: timer->CCR1 = duty; break;
        case PWM_CHANNEL_2: timer->CCR2 = duty; break;
        case PWM_CHANNEL_3: timer->CCR3 = duty; break;
        case PWM_CHANNEL_4: timer->CCR4 = duty; break;
    }
}

/* ------------------------------------------------------------------ */
uint32 Pwm_GetPeriod(TIM_TypeDef *timer)
{
    return timer->ARR;
}
