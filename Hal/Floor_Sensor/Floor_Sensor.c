/**
 * @file    Floor_Sensor.c
 * @brief   HAL — Floor-sensor implementation.
 *
 * ┌──────────────────────────────────────────────────────────────┐
 * │  Conflict-Free Pin Mapping (active-low, pull-up enabled)    │
 * │                                                              │
 * │  Floor 1 sensor  →  PB12  (EXTI12)                          │
 * │  Floor 2 sensor  →  PB13  (EXTI13)                          │
 * │  Floor 3 sensor  →  PB14  (EXTI14)                          │
 * │  Floor 4 sensor  →  PB15  (EXTI15)                          │
 * │                                                              │
 * │  All four lines share EXTI15_10_IRQn (Consolidated).        │
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

    /* Floor 1 → PB12 */
    sens_cfg.Pin = 12U;   Gpio_ConfigPin(&sens_cfg);
    /* Floor 2 → PB13 */
    sens_cfg.Pin = 13U;   Gpio_ConfigPin(&sens_cfg);
    /* Floor 3 → PB14 */
    sens_cfg.Pin = 14U;   Gpio_ConfigPin(&sens_cfg);
    /* Floor 4 → PB15 */
    sens_cfg.Pin = 15U;   Gpio_ConfigPin(&sens_cfg);

    /* ---- EXTI (SYSCFG clock assumed already enabled by PushButton) */
    Exti_CfgType exti;
    exti.Trigger = EXTI_TRIGGER_FALLING;  /* active-low sensors */
    exti.Port    = EXTI_PORT_B;

    exti.Line = 12U;   Exti_ConfigLine(&exti);
    exti.Line = 13U;   Exti_ConfigLine(&exti);
    exti.Line = 14U;   Exti_ConfigLine(&exti);
    exti.Line = 15U;   Exti_ConfigLine(&exti);
    
    /* NOTE: EXTI15_10_IRQn is enabled and configured by Push_Button.c
     * because it shares the line with the Emergency Stop (PD11).
     */
}

/* =================================================================== */
/*  Getter and Setter                                                  */
/* =================================================================== */
uint8 FloorSensor_GetCurrentFloor(void)
{
    return g_CurrentFloor;
}

void FloorSensor_SetCurrentFloor(uint8 floor_num)
{
    if (floor_num >= 1U && floor_num <= 4U) {
        g_CurrentFloor = floor_num;
    }
}
