/**
 * @file    Elevator_Motor.c
 * @brief   HAL — Motor implementation via Software PWM on PA0.
 */

#include "Elevator_Motor.h"
#include "Gpio.h"
#include "Mcu_Hw.h"

/* ---- Motor GPIO configuration ------------------------------------ */
#define MOTOR_GPIO_PORT     GPIOA
#define MOTOR_GPIO_PIN      0U

/* ---- Software PWM State ------------------------------------------ */
static volatile uint8 g_SoftwarePwm_Duty = 0U;

/* =================================================================== */
/*  Initialisation                                                     */
/* =================================================================== */
void ElevatorMotor_Init(void)
{
    /* 1. Enable GPIOA Clock */
    Gpio_EnableClock(MOTOR_GPIO_PORT);

    /* 2. Configure PA0 as General Purpose Output (Push-Pull) */
    Gpio_CfgType pin_cfg = {
        .Port    = MOTOR_GPIO_PORT,
        .Pin     = MOTOR_GPIO_PIN,
        .Mode    = GPIO_MODE_OUTPUT,
        .OType   = GPIO_OTYPE_PUSHPULL,
        .PuPd    = GPIO_PUPD_NONE,
        .Speed   = GPIO_SPEED_HIGH,
        .AltFunc = 0U /* Not used in output mode */
    };
    Gpio_ConfigPin(&pin_cfg);

    /* 3. Start with motor stopped */
    Elevator_SetSpeed(MOTOR_SPEED_STOP);
}

/* =================================================================== */
/*  Speed control                                                      */
/* =================================================================== */
void Elevator_SetSpeed(uint8 speed_pct)
{
    /* User specified presets: STOP=0, SLOW=2, FULL=10 */
    if (speed_pct > 10U) {
        speed_pct = 10U;
    }
    g_SoftwarePwm_Duty = speed_pct;
}

/* =================================================================== */
/*  Software PWM Engine (Called every 1ms from SysTick)                */
/* =================================================================== */
void ElevatorMotor_SoftwarePwmTick(void)
{
    static uint8 count = 0U;

    /* Cycle from 0 to 9 */
    if (count < g_SoftwarePwm_Duty) {
        Gpio_WriteHigh(MOTOR_GPIO_PORT, MOTOR_GPIO_PIN);
    } else {
        Gpio_WriteLow(MOTOR_GPIO_PORT, MOTOR_GPIO_PIN);
    }

    count++;
    if (count >= 10U) {
        count = 0U;
    }
}
