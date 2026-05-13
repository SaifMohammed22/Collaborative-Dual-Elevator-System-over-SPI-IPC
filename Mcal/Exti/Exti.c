/**
 * @file    Exti.c
 * @brief   MCAL — EXTI driver implementation.
 */

#include "Exti.h"
#include "Bit_Operations.h"

/* ------------------------------------------------------------------ */
void Exti_EnableSysCfgClock(void)
{
    SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SYSCFGEN_Pos);
}

/* ------------------------------------------------------------------ */
void Exti_ConfigLine(const Exti_CfgType *cfg)
{
    uint8 line = cfg->Line;

    /* --- SYSCFG: map port to EXTI line ----------------------------- */
    uint8 reg_idx  = line / 4U;           /* EXTICR[0..3] */
    uint8 field    = (line % 4U) * 4U;    /* 4-bit field position */

    SYSCFG->EXTICR[reg_idx] &= ~(0x0FU << field);
    SYSCFG->EXTICR[reg_idx] |=  ((uint32)cfg->Port << field);

    /* --- Trigger edge ---------------------------------------------- */
    switch (cfg->Trigger) {
        case EXTI_TRIGGER_RISING:
            SET_BIT(EXTI->RTSR, line);
            CLR_BIT(EXTI->FTSR, line);
            break;
        case EXTI_TRIGGER_FALLING:
            CLR_BIT(EXTI->RTSR, line);
            SET_BIT(EXTI->FTSR, line);
            break;
        case EXTI_TRIGGER_BOTH:
            SET_BIT(EXTI->RTSR, line);
            SET_BIT(EXTI->FTSR, line);
            break;
    }

    /* --- Unmask ---------------------------------------------------- */
    SET_BIT(EXTI->IMR, line);
}

/* ------------------------------------------------------------------ */
void Exti_ClearPending(uint8 line)
{
    /* Write 1 to clear (rc_w1) */
    EXTI->PR = (1U << line);
}
