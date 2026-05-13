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

#include "Cabin_Button.h"

/* ---- MCAL includes (hidden from the HAL header) ------------------- */
#include "Gpio.h"
#include "Exti.h"
#include "Nvic.h"
#include "Critical_Section.h"

/* =================================================================== */
/*  Volatile ISR ↔ main-loop shared flags                              */
/* =================================================================== */
static volatile uint8 g_BtnPressed[BTN_COUNT];
static volatile uint8 g_EmergencyActive;

/* =================================================================== */
/*  EXTI Callbacks                                                     */
/* =================================================================== */
static void Cabin_Floor1_Cb(void)
{
    g_BtnPressed[BTN_CABIN_FLOOR1] = TRUE;
}

static void Cabin_Floor2_Cb(void)
{
    g_BtnPressed[BTN_CABIN_FLOOR2] = TRUE;
}

static void Cabin_Floor3_Cb(void)
{
    g_BtnPressed[BTN_CABIN_FLOOR3] = TRUE;
}

static void Cabin_Floor4_Cb(void)
{
    g_BtnPressed[BTN_CABIN_FLOOR4] = TRUE;
}

static void Cabin_Emergency_Cb(void)
{
    g_EmergencyActive  = TRUE;
    g_BtnPressed[BTN_EMERGENCY_STOP] = TRUE;
}

/* =================================================================== */
/*  Initialisation                                                     */
/* =================================================================== */
void PushButton_Init(void)
{
    /* ---- GPIO clocks --------------------------------------------- */
    Gpio_EnableClock(GPIOC);
    Gpio_EnableClock(GPIOD);

    /* ---- GPIO: input with pull-up -------------------------------- */
    Gpio_CfgType btn_cfg = {
        .Mode   = GPIO_MODE_INPUT,
        .OType  = GPIO_OTYPE_PUSHPULL,
        .PuPd   = GPIO_PUPD_PU,
        .Speed  = GPIO_SPEED_LOW,
        .AltFunc = 0U
    };

    /* Cabin Floor 1 — PC6 */
    btn_cfg.Port = GPIOC;  btn_cfg.Pin = 6U;
    Gpio_ConfigPin(&btn_cfg);

    /* Cabin Floor 2 — PC7 */
    btn_cfg.Port = GPIOC;  btn_cfg.Pin = 7U;
    Gpio_ConfigPin(&btn_cfg);

    /* Cabin Floor 3 — PC8 */
    btn_cfg.Port = GPIOC;  btn_cfg.Pin = 8U;
    Gpio_ConfigPin(&btn_cfg);

    /* Cabin Floor 4 — PC9 */
    btn_cfg.Port = GPIOC;  btn_cfg.Pin = 9U;
    Gpio_ConfigPin(&btn_cfg);

    /* Emergency Stop — PD11 */
    btn_cfg.Port = GPIOD;  btn_cfg.Pin = 11U;
    Gpio_ConfigPin(&btn_cfg);

    /* ---- EXTI ---------------------------------------------------- */
    Exti_EnableSysCfgClock();

    Exti_CfgType exti;
    exti.Trigger = EXTI_TRIGGER_FALLING;

    /* EXTI6 → PC6 (Floor 1) */
    exti.Line = 6U;   exti.Port = EXTI_PORT_C;  Exti_ConfigLine(&exti);
    Exti_SetCallback(6U, Cabin_Floor1_Cb);

    /* EXTI7 → PC7 (Floor 2) */
    exti.Line = 7U;   exti.Port = EXTI_PORT_C;  Exti_ConfigLine(&exti);
    Exti_SetCallback(7U, Cabin_Floor2_Cb);

    /* EXTI8 → PC8 (Floor 3) */
    exti.Line = 8U;   exti.Port = EXTI_PORT_C;  Exti_ConfigLine(&exti);
    Exti_SetCallback(8U, Cabin_Floor3_Cb);

    /* EXTI9 → PC9 (Floor 4) */
    exti.Line = 9U;   exti.Port = EXTI_PORT_C;  Exti_ConfigLine(&exti);
    Exti_SetCallback(9U, Cabin_Floor4_Cb);

    /* EXTI11 → PD11 (Emergency Stop) */
    exti.Line = 11U;  exti.Port = EXTI_PORT_D;  Exti_ConfigLine(&exti);
    Exti_SetCallback(11U, Cabin_Emergency_Cb);

    /* ---- NVIC ---------------------------------------------------- */
    /* 4 bits pre-emption / 0 sub (grouping = 3) */
    Nvic_SetPriorityGrouping(3U);

    /*  Emergency stop gets pre-emption priority 0 (HIGHEST).
     *  Cabin buttons get priority 5 (comfortably lower).              */
    Nvic_SetPriority(EXTI9_5_IRQn,   5U, 0U);
    Nvic_SetPriority(EXTI15_10_IRQn, 0U, 0U);  /* ← highest for emergency (line 11 is in 15_10) */

    Nvic_EnableIrq(EXTI9_5_IRQn);
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
    ENTER_CRITICAL();
    uint8 was_pressed = g_BtnPressed[id];
    g_BtnPressed[id] = FALSE;
    EXIT_CRITICAL();

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
