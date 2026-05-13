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
/*  EXTI Callbacks                                                     */
/* =================================================================== */
static void Floor1_Cb(void) { g_CurrentFloor = 1U; }
static void Floor2_Cb(void) { g_CurrentFloor = 2U; }
static void Floor3_Cb(void) { g_CurrentFloor = 3U; }
static void Floor4_Cb(void) { g_CurrentFloor = 4U; }

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
    sens_cfg.Pin = 12U;  Gpio_ConfigPin(&sens_cfg);
    /* Floor 2 → PB13 */
    sens_cfg.Pin = 13U;  Gpio_ConfigPin(&sens_cfg);
    /* Floor 3 → PB14 */
    sens_cfg.Pin = 14U;  Gpio_ConfigPin(&sens_cfg);
    /* Floor 4 → PB15 */
    sens_cfg.Pin = 15U;  Gpio_ConfigPin(&sens_cfg);

    /* ---- EXTI ---------------------------------------------------- */
    Exti_EnableSysCfgClock();

    Exti_CfgType exti;
    exti.Trigger = EXTI_TRIGGER_FALLING;  /* active-low sensors */
    exti.Port    = EXTI_PORT_B;

    exti.Line = 12U;  Exti_ConfigLine(&exti);  Exti_SetCallback(12U, Floor1_Cb);
    exti.Line = 13U;  Exti_ConfigLine(&exti);  Exti_SetCallback(13U, Floor2_Cb);
    exti.Line = 14U;  Exti_ConfigLine(&exti);  Exti_SetCallback(14U, Floor3_Cb);
    exti.Line = 15U;  Exti_ConfigLine(&exti);  Exti_SetCallback(15U, Floor4_Cb);

    /* ---- NVIC — medium priority (3) ------------------------------ */
    Nvic_SetPriority(EXTI15_10_IRQn, 3U, 0U);
    Nvic_EnableIrq(EXTI15_10_IRQn);
}

/* =================================================================== */
/*  Getter                                                             */
/* =================================================================== */
uint8 FloorSensor_GetCurrentFloor(void)
{
    return g_CurrentFloor;
}
