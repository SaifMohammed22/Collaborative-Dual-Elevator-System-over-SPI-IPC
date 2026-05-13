/**
 * @file    Floor_Sensor.c
 * @brief   HAL — Floor-sensor implementation.
 *
 * ┌──────────────────────────────────────────────────────────────┐
 * │  Default Pin Mapping  (active-low, pull-up enabled)         │
 * │                                                              │
 * │  Floor 1 sensor  →  PB5   (EXTI5)                           │
 * │  Floor 2 sensor  →  PB6   (EXTI6)                           │
 * │  Floor 3 sensor  →  PB7   (EXTI7)                           │
 * │  Floor 4 sensor  →  PB8   (EXTI8)                           │
 * │                                                              │
 * │  All four lines share EXTI9_5_IRQn.                         │
 * │  Adjust the mapping below to match your PCB.                │
 * └──────────────────────────────────────────────────────────────┘
 */

#include "Floor_Sensor.h"

/* ---- MCAL includes (hidden from the HAL header) ------------------- */
#include "Gpio.h"
#include "Exti.h"
#include "Nvic.h"

/* =================================================================== */
/*  Volatile shared state                                              */
/* =================================================================== */
static volatile uint8 g_CurrentFloor = 1U;

/* =================================================================== */
/*  Initialisation                                                     */
/* =================================================================== */
void FloorSensor_Init(void)
{
    Gpio_EnableClock(GPIOB);

    Gpio_CfgType sens_cfg = {
        .Port    = GPIOB,
        .Mode    = GPIO_MODE_INPUT,
        .OType   = GPIO_OTYPE_PUSHPULL,
        .PuPd    = GPIO_PUPD_PU,
        .Speed   = GPIO_SPEED_LOW,
        .AltFunc = 0U
    };

    /* Floor 1 → PB5 */
    sens_cfg.Pin = 5U;   Gpio_ConfigPin(&sens_cfg);
    /* Floor 2 → PB6 */
    sens_cfg.Pin = 6U;   Gpio_ConfigPin(&sens_cfg);
    /* Floor 3 → PB7 */
    sens_cfg.Pin = 7U;   Gpio_ConfigPin(&sens_cfg);
    /* Floor 4 → PB8 */
    sens_cfg.Pin = 8U;   Gpio_ConfigPin(&sens_cfg);

    /* ---- EXTI (SYSCFG clock assumed already enabled by PushButton) */
    Exti_CfgType exti;
    exti.Trigger = EXTI_TRIGGER_FALLING;  /* active-low sensors */
    exti.Port    = EXTI_PORT_B;

    exti.Line = 5U;   Exti_ConfigLine(&exti);
    exti.Line = 6U;   Exti_ConfigLine(&exti);
    exti.Line = 7U;   Exti_ConfigLine(&exti);
    exti.Line = 8U;   Exti_ConfigLine(&exti);

    /* ---- NVIC — medium priority (3) ------------------------------ */
    Nvic_SetPriority(EXTI9_5_IRQn, 3U, 0U);
    Nvic_EnableIrq(EXTI9_5_IRQn);
}

/* =================================================================== */
/*  Getter                                                             */
/* =================================================================== */
uint8 FloorSensor_GetCurrentFloor(void)
{
    return g_CurrentFloor;
}

/* =================================================================== */
/*  EXTI9_5 ISR — covers PB5 … PB8                                    */
/* =================================================================== */
#if 0
void EXTI9_5_IRQHandler(void)
{
    if (EXTI->PR & (1U << 5U)) {
        Exti_ClearPending(5U);
        g_CurrentFloor = 1U;
    }
    if (EXTI->PR & (1U << 6U)) {
        Exti_ClearPending(6U);
        g_CurrentFloor = 2U;
    }
    if (EXTI->PR & (1U << 7U)) {
        Exti_ClearPending(7U);
        g_CurrentFloor = 3U;
    }
    if (EXTI->PR & (1U << 8U)) {
        Exti_ClearPending(8U);
        g_CurrentFloor = 4U;
    }
}
#endif
