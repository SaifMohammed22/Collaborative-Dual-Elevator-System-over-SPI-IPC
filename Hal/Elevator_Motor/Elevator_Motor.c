/**
 * @file    Elevator_Motor.c
 * @brief   HAL — Motor (LED/PWM) implementation.
 *
 * ┌─────────────────────────────────────────────────────────┐
 * │  Default Pin & Timer                                    │
 * │                                                         │
 * │  Motor LED  →  PA6  (TIM3 Channel 1,  AF2)             │
 * │                                                         │
 * │  Prescaler  = 15   → TIM3 tick = 1 MHz  (16 MHz / 16)  │
 * │  ARR        = 999  → PWM freq  ≈ 1 kHz                 │
 * │                                                         │
 * │  Adjust to your actual clock tree / pin assignment.     │
 * └─────────────────────────────────────────────────────────┘
 */

#include "Elevator_Motor.h"

/* ---- MCAL includes (hidden from the HAL header) ------------------- */
#include "Gpio.h"
#include "Pwm.h"

/* ---- Motor timer configuration ------------------------------------ */
#define MOTOR_TIMER         TIM3
#define MOTOR_CHANNEL       PWM_CHANNEL_1
#define MOTOR_GPIO_PORT     GPIOA
#define MOTOR_GPIO_PIN      6U
#define MOTOR_GPIO_AF       2U          /* AF2 = TIM3 on PA6 */

#define MOTOR_PSC           15U         /* prescaler (16 MHz / 16 = 1 MHz) */
#define MOTOR_ARR           999U        /* auto-reload → ~1 kHz PWM */

/* =================================================================== */
/*  Initialisation                                                     */
/* =================================================================== */
void ElevatorMotor_Init(void)
{
    /* ---- GPIO: AF push-pull for PWM output ----------------------- */
    Gpio_EnableClock(MOTOR_GPIO_PORT);

    Gpio_CfgType pin_cfg = {
        .Port    = MOTOR_GPIO_PORT,
        .Pin     = MOTOR_GPIO_PIN,
        .Mode    = GPIO_MODE_AF,
        .OType   = GPIO_OTYPE_PUSHPULL,
        .PuPd    = GPIO_PUPD_NONE,
        .Speed   = GPIO_SPEED_HIGH,
        .AltFunc = MOTOR_GPIO_AF
    };
    Gpio_ConfigPin(&pin_cfg);

    /* ---- Timer / PWM --------------------------------------------- */
    Pwm_CfgType pwm_cfg = {
        .Timer     = MOTOR_TIMER,
        .Channel   = MOTOR_CHANNEL,
        .Prescaler = MOTOR_PSC,
        .Period    = MOTOR_ARR
    };
    Pwm_Init(&pwm_cfg);

    /* Start with motor stopped */
    Elevator_SetSpeed(MOTOR_SPEED_STOP);
}

/* =================================================================== */
/*  Speed control                                                      */
/* =================================================================== */
void Elevator_SetSpeed(uint8_t speed_pct)
{
    /* Clamp to 0 – 100 */
    if (speed_pct > 100U) {
        speed_pct = 100U;
    }

    uint32_t arr  = Pwm_GetPeriod(MOTOR_TIMER);
    uint32_t duty = ((uint32_t)speed_pct * (arr + 1U)) / 100U;

    Pwm_SetDuty(MOTOR_TIMER, MOTOR_CHANNEL, duty);
}
