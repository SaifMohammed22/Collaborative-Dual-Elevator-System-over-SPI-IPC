/**
 * @file    Pwm.h
 * @brief   MCAL — PWM (Timer OC) driver for STM32F401xE.
 *
 * Targets a single general-purpose timer channel in PWM mode 1.
 * The caller selects the timer instance, channel, prescaler, and ARR
 * at init time; afterwards only the duty cycle changes at runtime.
 */

#ifndef PWM_H_
#define PWM_H_

#include "Std_Types.h"
#include "Mcu_Hw.h"

/* ------------------------------------------------------------------ */
/*  Channel index                                                      */
/* ------------------------------------------------------------------ */
typedef enum {
    PWM_CHANNEL_1 = 0U,
    PWM_CHANNEL_2 = 1U,
    PWM_CHANNEL_3 = 2U,
    PWM_CHANNEL_4 = 3U
} Pwm_ChannelType;

/* ------------------------------------------------------------------ */
/*  Configuration                                                      */
/* ------------------------------------------------------------------ */
typedef struct {
    TIM_TypeDef     *Timer;       /* e.g. TIM2, TIM3, TIM4 …           */
    Pwm_ChannelType  Channel;
    uint16_t         Prescaler;   /* TIMx_PSC value                    */
    uint32_t         Period;      /* TIMx_ARR value (auto-reload)      */
} Pwm_CfgType;

/* ------------------------------------------------------------------ */
/*  API                                                                */
/* ------------------------------------------------------------------ */

/**
 * @brief Enable the APB timer clock and configure the channel in
 *        PWM Mode 1 with an initial duty of 0%.
 */
void Pwm_Init(const Pwm_CfgType *cfg);

/**
 * @brief Change the duty cycle (CCRx) of the already-initialised channel.
 * @param timer    Timer peripheral.
 * @param channel  Channel index.
 * @param duty     New CCR value (0 … ARR).
 */
void Pwm_SetDuty(TIM_TypeDef *timer, Pwm_ChannelType channel, uint32_t duty);

/**
 * @brief Return the current ARR value (useful for % calculation).
 */
uint32_t Pwm_GetPeriod(TIM_TypeDef *timer);

#endif /* PWM_H_ */
