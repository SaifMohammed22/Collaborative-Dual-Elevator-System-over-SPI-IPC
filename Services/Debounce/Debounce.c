#include "Debounce.h"
#include "Critical_Section.h"

#define DEBOUNCE_COOLDOWN_TICKS 5U

static uint8 g_Cooldown[6] = {0U};
static volatile uint8 g_ConfirmedPress[6] = {0U};

void Debounce_Process(void)
{
    for (uint8 i = 0U; i < 6U; i++) {
        /* Decrement cooldown if active */
        if (g_Cooldown[i] > 0U) {
            g_Cooldown[i]--;
        }
        
        /* Check raw EXTI flag from the HAL */
        uint8 raw_flag = HAL_HallwayButton_GetRawFlag((HallwayButton_Id_t)i);
        
        /* If a press happened and cooldown is 0, register a confirmed press */
        if ((raw_flag == 1U) && (g_Cooldown[i] == 0U)) {
            g_ConfirmedPress[i] = 1U;
            g_Cooldown[i] = DEBOUNCE_COOLDOWN_TICKS;
        }
    }
}

uint8 Debounce_GetConfirmedCall(HallwayButton_Id_t btnId)
{
    if ((uint8)btnId >= 6U) return 0U;
    
    ENTER_CRITICAL();
    uint8 is_pressed = g_ConfirmedPress[(uint8)btnId];
    g_ConfirmedPress[(uint8)btnId] = 0U;
    EXIT_CRITICAL();
    
    return is_pressed;
}
