#include "Hallway_Button.h"
#include "Gpio.h"
#include "Exti.h"
#include "Nvic.h"
#include "Critical_Section.h"

static volatile uint8 g_RawHallwayFlags[6] = {0U};

/* --- EXTI Callbacks --- */
static void Hallway_Cb0(void) { g_RawHallwayFlags[0] = 1U; }
static void Hallway_Cb1(void) { g_RawHallwayFlags[1] = 1U; }
static void Hallway_Cb2(void) { g_RawHallwayFlags[2] = 1U; }
static void Hallway_Cb3(void) { g_RawHallwayFlags[3] = 1U; }
static void Hallway_Cb4(void) { g_RawHallwayFlags[4] = 1U; }
static void Hallway_Cb5(void) { g_RawHallwayFlags[5] = 1U; }

void HAL_HallwayButton_Init(void)
{
    Gpio_EnableClock(GPIOB);

    Gpio_CfgType btn_cfg = {
        .Port    = GPIOB,
        .Mode    = GPIO_MODE_INPUT,
        .OType   = GPIO_OTYPE_PUSHPULL,
        .PuPd    = GPIO_PUPD_PU,
        .Speed   = GPIO_SPEED_LOW,
        .AltFunc = 0U
    };

    /* PB0-PB5 */
    for (uint8 pin = 0U; pin <= 5U; pin++) {
        btn_cfg.Pin = pin;
        Gpio_ConfigPin(&btn_cfg);
    }

    Exti_EnableSysCfgClock();

    Exti_CfgType exti;
    exti.Trigger = EXTI_TRIGGER_FALLING;
    exti.Port    = EXTI_PORT_B;

    /* EXTI 0-5 */
    for (uint8 line = 0U; line <= 5U; line++) {
        exti.Line = line;
        Exti_ConfigLine(&exti);
    }

    Exti_SetCallback(0U, Hallway_Cb0);
    Exti_SetCallback(1U, Hallway_Cb1);
    Exti_SetCallback(2U, Hallway_Cb2);
    Exti_SetCallback(3U, Hallway_Cb3);
    Exti_SetCallback(4U, Hallway_Cb4);
    Exti_SetCallback(5U, Hallway_Cb5);

    /* Priority 5 (same as Cabin buttons) */
    Nvic_SetPriority(EXTI0_IRQn,   5U, 0U);
    Nvic_SetPriority(EXTI1_IRQn,   5U, 0U);
    Nvic_SetPriority(EXTI2_IRQn,   5U, 0U);
    Nvic_SetPriority(EXTI3_IRQn,   5U, 0U);
    Nvic_SetPriority(EXTI4_IRQn,   5U, 0U);
    Nvic_SetPriority(EXTI9_5_IRQn, 5U, 0U); /* For EXTI5 */

    Nvic_EnableIrq(EXTI0_IRQn);
    Nvic_EnableIrq(EXTI1_IRQn);
    Nvic_EnableIrq(EXTI2_IRQn);
    Nvic_EnableIrq(EXTI3_IRQn);
    Nvic_EnableIrq(EXTI4_IRQn);
    Nvic_EnableIrq(EXTI9_5_IRQn);
}

uint8 HAL_HallwayButton_GetRawFlag(HallwayButton_Id_t btnId)
{
    if ((uint8)btnId >= 6U) return 0U;
    
    ENTER_CRITICAL();
    uint8 flag = g_RawHallwayFlags[(uint8)btnId];
    g_RawHallwayFlags[(uint8)btnId] = 0U;
    EXIT_CRITICAL();
    
    return flag;
}
