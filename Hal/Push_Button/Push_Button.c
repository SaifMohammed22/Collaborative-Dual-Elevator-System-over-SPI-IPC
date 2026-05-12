/**
 * @file    Push_Button.c
 * @brief   HAL — Push-button implementation.
 *
 * ┌─────────────────────────────────────────────────────────────────┐
 * │  Default Pin Mapping  (active-low with internal pull-up)       │
 * │                                                                 │
 * │  Cabin Floor 1   →  PA0   (EXTI0)                              │
 * │  Cabin Floor 2   →  PA1   (EXTI1)                              │
 * │  Cabin Floor 3   →  PA4   (EXTI4)                              │
 * │  Cabin Floor 4   →  PB2   (EXTI2)                              │
 * │  Emergency Stop  →  PC13  (EXTI13)  ← highest NVIC priority   │
 * │                                                                 │
 * │  Each button has a unique EXTI line — no mux conflicts.        │
 * │  Adjust the mapping below to match your PCB.                   │
 * └─────────────────────────────────────────────────────────────────┘
 *
 *  IMPORTANT — Concurrency:
 *      All flags shared between ISRs and the main loop are declared
 *      `volatile`.  Reads / clears use a critical section where the
 *      flag must be consumed atomically.
 */

#include "Push_Button.h"

/* ---- MCAL includes (hidden from the HAL header) ------------------- */
#include "Gpio.h"
#include "Exti.h"
#include "Nvic.h"

/* =================================================================== */
/*  Volatile ISR ↔ main-loop shared flags                              */
/* =================================================================== */
static volatile uint8 g_BtnPressed[BTN_COUNT];
static volatile uint8 g_EmergencyActive;

/* =================================================================== */
/*  Initialisation                                                     */
/* =================================================================== */
void PushButton_Init(void)
{
    /* ---- GPIO clocks --------------------------------------------- */
    Gpio_EnableClock(GPIOA);
    Gpio_EnableClock(GPIOB);
    Gpio_EnableClock(GPIOC);

    /* ---- GPIO: input with pull-up -------------------------------- */
    Gpio_CfgType btn_cfg = {
        .Mode   = GPIO_MODE_INPUT,
        .OType  = GPIO_OTYPE_PUSHPULL,
        .PuPd   = GPIO_PUPD_PU,
        .Speed  = GPIO_SPEED_LOW,
        .AltFunc = 0U
    };

    /* Cabin Floor 1 — PA0 */
    btn_cfg.Port = GPIOA;  btn_cfg.Pin = 0U;
    Gpio_ConfigPin(&btn_cfg);

    /* Cabin Floor 2 — PA1 */
    btn_cfg.Port = GPIOA;  btn_cfg.Pin = 1U;
    Gpio_ConfigPin(&btn_cfg);

    /* Cabin Floor 3 — PA4 */
    btn_cfg.Port = GPIOA;  btn_cfg.Pin = 4U;
    Gpio_ConfigPin(&btn_cfg);

    /* Cabin Floor 4 — PB2 */
    btn_cfg.Port = GPIOB;  btn_cfg.Pin = 2U;
    Gpio_ConfigPin(&btn_cfg);

    /* Emergency Stop — PC13 */
    btn_cfg.Port = GPIOC;  btn_cfg.Pin = 13U;
    Gpio_ConfigPin(&btn_cfg);

    /* ---- EXTI ---------------------------------------------------- */
    Exti_EnableSysCfgClock();

    /* Each button has a dedicated EXTI line — one port per line,
     * no SYSCFG mux conflicts.                                       */

    Exti_CfgType exti;
    exti.Trigger = EXTI_TRIGGER_FALLING;

    /* EXTI0 → PA0 (Floor 1) */
    exti.Line = 0U;   exti.Port = EXTI_PORT_A;  Exti_ConfigLine(&exti);

    /* EXTI1 → PA1 (Floor 2) */
    exti.Line = 1U;   exti.Port = EXTI_PORT_A;  Exti_ConfigLine(&exti);

    /* EXTI4 → PA4 (Floor 3) */
    exti.Line = 4U;   exti.Port = EXTI_PORT_A;  Exti_ConfigLine(&exti);

    /* EXTI2 → PB2 (Floor 4) */
    exti.Line = 2U;   exti.Port = EXTI_PORT_B;  Exti_ConfigLine(&exti);

    /* EXTI13 → PC13 (Emergency Stop) */
    exti.Line = 13U;  exti.Port = EXTI_PORT_C;  Exti_ConfigLine(&exti);

    /* ---- NVIC ---------------------------------------------------- */
    /* 4 bits pre-emption / 0 sub (grouping = 3) */
    Nvic_SetPriorityGrouping(3U);

    /*  Emergency stop gets pre-emption priority 0 (HIGHEST).
     *  Cabin buttons get priority 5 (comfortably lower).              */
    Nvic_SetPriority(EXTI0_IRQn,     5U, 0U);
    Nvic_SetPriority(EXTI1_IRQn,     5U, 0U);
    Nvic_SetPriority(EXTI2_IRQn,     5U, 0U);
    Nvic_SetPriority(EXTI4_IRQn,     5U, 0U);
    Nvic_SetPriority(EXTI15_10_IRQn, 0U, 0U);  /* ← highest for emergency */

    Nvic_EnableIrq(EXTI0_IRQn);
    Nvic_EnableIrq(EXTI1_IRQn);
    Nvic_EnableIrq(EXTI2_IRQn);
    Nvic_EnableIrq(EXTI4_IRQn);
    Nvic_EnableIrq(EXTI15_10_IRQn);

    /* ---- Clear all flags ----------------------------------------- */
    for (uint8 i = 0U; i < BTN_COUNT; i++) {
        g_BtnPressed[i] = FALSE;
    }
    g_EmergencyActive = FALSE;
}

/* =================================================================== */
/*  Flag access (main-loop side)                                       */
/* =================================================================== */
uint8 PushButton_GetAndClear(PushButton_IdType id)
{
    if (id >= BTN_COUNT) { return FALSE; }

    /* Atomic read-then-clear inside a critical section */
    __asm volatile("cpsid i");
    uint8 was_pressed = g_BtnPressed[id];
    g_BtnPressed[id] = FALSE;
    __asm volatile("cpsie i");

    return was_pressed;
}

uint8 PushButton_IsEmergencyActive(void)
{
    return g_EmergencyActive;
}

void PushButton_ClearEmergency(void)
{
    g_EmergencyActive = FALSE;
}

/* =================================================================== */
/*  EXTI ISR handlers (weak symbols overridden here)                   */
/* =================================================================== */

/**
 * @brief  EXTI0 ISR — PA0 (Cabin Floor 1).
 */
void EXTI0_IRQHandler(void)
{
    Exti_ClearPending(0U);
    g_BtnPressed[BTN_CABIN_FLOOR1] = TRUE;
}

/**
 * @brief  EXTI2 ISR — PB2 (Cabin Floor 4).
 */
void EXTI2_IRQHandler(void)
{
    Exti_ClearPending(2U);
    g_BtnPressed[BTN_CABIN_FLOOR4] = TRUE;
}

/**
 * @brief  EXTI1 ISR — PA1 (Floor 2).
 */
void EXTI1_IRQHandler(void)
{
    Exti_ClearPending(1U);
    g_BtnPressed[BTN_CABIN_FLOOR2] = TRUE;
}

/**
 * @brief  EXTI4 ISR — PA4 (Floor 3).
 */
void EXTI4_IRQHandler(void)
{
    Exti_ClearPending(4U);
    g_BtnPressed[BTN_CABIN_FLOOR3] = TRUE;
}

/**
 * @brief  EXTI15_10 ISR — PC13 (Emergency Stop).
 *         Highest pre-emption priority — cannot be interrupted.
 */
void EXTI15_10_IRQHandler(void)
{
    if (EXTI->PR & (1U << 13U)) {
        Exti_ClearPending(13U);
        g_EmergencyActive  = TRUE;
        g_BtnPressed[BTN_EMERGENCY_STOP] = TRUE;
    }
}
