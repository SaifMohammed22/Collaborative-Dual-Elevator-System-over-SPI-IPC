#include "Push_Button.h"
#include "Gpio.h"
#include "Exti.h"
#include "Nvic.h"
#include "Critical_Section.h"

/* =================================================================== */
/*  Volatile ISR ↔ main-loop shared flags                              */
/* =================================================================== */
static volatile uint8 g_BtnPressed[11];
static volatile uint8 g_EmergencyActive;

/* =================================================================== */
/*  Software Debounce State                                            */
/* =================================================================== */
extern volatile uint32 g_SysTick_Ms;
static uint32 last_interrupt_time[11] = {0};

static uint32 Tick_GetMs(void) {
    return g_SysTick_Ms;
}

/* =================================================================== */
/*  Initialisation                                                     */
/* =================================================================== */
void PushButton_Init(void)
{
    /* ---- GPIO clocks --------------------------------------------- */
    Gpio_EnableClock(GPIOB);
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

    /* --- Hallway Buttons (PB0-PB5) --- */
    btn_cfg.Port = GPIOB;
    for (uint8 pin = 0U; pin <= 5U; pin++) {
        btn_cfg.Pin = pin;
        Gpio_ConfigPin(&btn_cfg);
    }

    /* --- Cabin Buttons (PC6-PC9) --- */
    btn_cfg.Port = GPIOC;
    for (uint8 pin = 6U; pin <= 9U; pin++) {
        btn_cfg.Pin = pin;
        Gpio_ConfigPin(&btn_cfg);
    }

    /* --- Emergency Stop (PD11) --- */
    btn_cfg.Port = GPIOD;
    btn_cfg.Pin = 11U;
    Gpio_ConfigPin(&btn_cfg);

    /* ---- EXTI ---------------------------------------------------- */
    Exti_EnableSysCfgClock();

    Exti_CfgType exti;
    exti.Trigger = EXTI_TRIGGER_FALLING;

    /* Hallway: EXTI0 - EXTI5 on PORTB */
    exti.Port = EXTI_PORT_B;
    for (uint8 line = 0U; line <= 5U; line++) {
        exti.Line = line;
        Exti_ConfigLine(&exti);
    }

    /* Cabin: EXTI6 - EXTI9 on PORTC */
    exti.Port = EXTI_PORT_C;
    for (uint8 line = 6U; line <= 9U; line++) {
        exti.Line = line;
        Exti_ConfigLine(&exti);
    }

    /* Emergency: EXTI11 on PORTD */
    exti.Port = EXTI_PORT_D;
    exti.Line = 11U;
    Exti_ConfigLine(&exti);

    /* ---- NVIC ---------------------------------------------------- */
    /* 4 bits pre-emption / 0 sub (grouping = 3) */
    Nvic_SetPriorityGrouping(3U);

    /* Emergency stop gets pre-emption priority 0 (HIGHEST).
     * Cabin/Hallway buttons get priority 5 (comfortably lower). */
    Nvic_SetPriority(EXTI0_IRQn,     5U, 0U);
    Nvic_SetPriority(EXTI1_IRQn,     5U, 0U);
    Nvic_SetPriority(EXTI2_IRQn,     5U, 0U);
    Nvic_SetPriority(EXTI3_IRQn,     5U, 0U);
    Nvic_SetPriority(EXTI4_IRQn,     5U, 0U);
    Nvic_SetPriority(EXTI9_5_IRQn,   5U, 0U);
    Nvic_SetPriority(EXTI15_10_IRQn, 0U, 0U);  /* ← highest for emergency (line 11) */

    Nvic_EnableIrq(EXTI0_IRQn);
    Nvic_EnableIrq(EXTI1_IRQn);
    Nvic_EnableIrq(EXTI2_IRQn);
    Nvic_EnableIrq(EXTI3_IRQn);
    Nvic_EnableIrq(EXTI4_IRQn);
    Nvic_EnableIrq(EXTI9_5_IRQn);
    Nvic_EnableIrq(EXTI15_10_IRQn);

    /* ---- Clear all flags ----------------------------------------- */
    for (uint8 i = 0U; i < 11U; i++) {
        g_BtnPressed[i] = FALSE;
        last_interrupt_time[i] = 0U;
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

/* =================================================================== */
/*  EXTI ISR handlers                                                  */
/* =================================================================== */

void EXTI0_IRQHandler(void)
{
    Exti_ClearPending(0U);
    if ((Tick_GetMs() - last_interrupt_time[BTN_HALLWAY_U1]) > 50U) {
        g_BtnPressed[BTN_HALLWAY_U1] = TRUE;
        last_interrupt_time[BTN_HALLWAY_U1] = Tick_GetMs();
    }
}

void EXTI1_IRQHandler(void)
{
    Exti_ClearPending(1U);
    if ((Tick_GetMs() - last_interrupt_time[BTN_HALLWAY_D2]) > 50U) {
        g_BtnPressed[BTN_HALLWAY_D2] = TRUE;
        last_interrupt_time[BTN_HALLWAY_D2] = Tick_GetMs();
    }
}

void EXTI2_IRQHandler(void)
{
    Exti_ClearPending(2U);
    if ((Tick_GetMs() - last_interrupt_time[BTN_HALLWAY_U2]) > 50U) {
        g_BtnPressed[BTN_HALLWAY_U2] = TRUE;
        last_interrupt_time[BTN_HALLWAY_U2] = Tick_GetMs();
    }
}

void EXTI3_IRQHandler(void)
{
    Exti_ClearPending(3U);
    if ((Tick_GetMs() - last_interrupt_time[BTN_HALLWAY_D3]) > 50U) {
        g_BtnPressed[BTN_HALLWAY_D3] = TRUE;
        last_interrupt_time[BTN_HALLWAY_D3] = Tick_GetMs();
    }
}

void EXTI4_IRQHandler(void)
{
    Exti_ClearPending(4U);
    if ((Tick_GetMs() - last_interrupt_time[BTN_HALLWAY_U3]) > 50U) {
        g_BtnPressed[BTN_HALLWAY_U3] = TRUE;
        last_interrupt_time[BTN_HALLWAY_U3] = Tick_GetMs();
    }
}

void EXTI9_5_IRQHandler(void)
{
    /* Hallway D4 (PB5) */
    if (EXTI->PR & (1U << 5U)) {
        Exti_ClearPending(5U);
        if ((Tick_GetMs() - last_interrupt_time[BTN_HALLWAY_D4]) > 50U) {
            g_BtnPressed[BTN_HALLWAY_D4] = TRUE;
            last_interrupt_time[BTN_HALLWAY_D4] = Tick_GetMs();
        }
    }
    /* Cabin Floor 1 (PC6) */
    if (EXTI->PR & (1U << 6U)) {
        Exti_ClearPending(6U);
        if ((Tick_GetMs() - last_interrupt_time[BTN_CABIN_FLOOR1]) > 50U) {
            g_BtnPressed[BTN_CABIN_FLOOR1] = TRUE;
            last_interrupt_time[BTN_CABIN_FLOOR1] = Tick_GetMs();
        }
    }
    /* Cabin Floor 2 (PC7) */
    if (EXTI->PR & (1U << 7U)) {
        Exti_ClearPending(7U);
        if ((Tick_GetMs() - last_interrupt_time[BTN_CABIN_FLOOR2]) > 50U) {
            g_BtnPressed[BTN_CABIN_FLOOR2] = TRUE;
            last_interrupt_time[BTN_CABIN_FLOOR2] = Tick_GetMs();
        }
    }
    /* Cabin Floor 3 (PC8) */
    if (EXTI->PR & (1U << 8U)) {
        Exti_ClearPending(8U);
        if ((Tick_GetMs() - last_interrupt_time[BTN_CABIN_FLOOR3]) > 50U) {
            g_BtnPressed[BTN_CABIN_FLOOR3] = TRUE;
            last_interrupt_time[BTN_CABIN_FLOOR3] = Tick_GetMs();
        }
    }
    /* Cabin Floor 4 (PC9) */
    if (EXTI->PR & (1U << 9U)) {
        Exti_ClearPending(9U);
        if ((Tick_GetMs() - last_interrupt_time[BTN_CABIN_FLOOR4]) > 50U) {
            g_BtnPressed[BTN_CABIN_FLOOR4] = TRUE;
            last_interrupt_time[BTN_CABIN_FLOOR4] = Tick_GetMs();
        }
    }
}

void EXTI15_10_IRQHandler(void)
{
    /* Emergency Stop (PD11) */
    if (EXTI->PR & (1U << 11U)) {
        Exti_ClearPending(11U);
        if ((Tick_GetMs() - last_interrupt_time[BTN_EMERGENCY_STOP]) > 50U) {
            g_EmergencyActive = TRUE;
            g_BtnPressed[BTN_EMERGENCY_STOP] = TRUE;
            last_interrupt_time[BTN_EMERGENCY_STOP] = Tick_GetMs();
        }
    }
}
