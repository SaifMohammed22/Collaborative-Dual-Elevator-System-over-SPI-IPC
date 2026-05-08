/**
 * @file    Nvic.c
 * @brief   MCAL — NVIC driver implementation.
 */

#include "Nvic.h"

/* ------------------------------------------------------------------ */
void Nvic_EnableIrq(IRQn_Type irqn)
{
    NVIC_EnableIRQ(irqn);      /* CMSIS intrinsic */
}

/* ------------------------------------------------------------------ */
void Nvic_DisableIrq(IRQn_Type irqn)
{
    NVIC_DisableIRQ(irqn);
}

/* ------------------------------------------------------------------ */
void Nvic_SetPriority(IRQn_Type irqn, uint8_t preempt, uint8_t sub)
{
    /*
     * Combine preempt and sub into the format expected by CMSIS based
     * on the current PRIGROUP setting.
     */
    uint32_t grouping     = NVIC_GetPriorityGrouping();
    uint32_t encoded_prio = NVIC_EncodePriority(grouping, preempt, sub);
    NVIC_SetPriority(irqn, encoded_prio);
}

/* ------------------------------------------------------------------ */
void Nvic_SetPriorityGrouping(uint32_t grouping)
{
    NVIC_SetPriorityGrouping(grouping);
}
