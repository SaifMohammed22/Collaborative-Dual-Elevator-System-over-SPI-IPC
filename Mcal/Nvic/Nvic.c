/**
 * @file    Nvic.c
 * @brief   MCAL — NVIC driver implementation.
 */

#include "Nvic.h"
#include "Mcu_Hw.h"

/* ------------------------------------------------------------------ */
void Nvic_EnableIrq(IRQn_Type irqn)
{
    if ((sint32)irqn >= 0)
    {
        NVIC->ISER[((uint32)irqn) >> 5] = (1UL << (((uint32)irqn) & 0x1F));
    }
}

/* ------------------------------------------------------------------ */
void Nvic_DisableIrq(IRQn_Type irqn)
{
    if ((sint32)irqn >= 0)
    {
        NVIC->ICER[((uint32)irqn) >> 5] = (1UL << (((uint32)irqn) & 0x1F));
    }
}

/* ------------------------------------------------------------------ */
void Nvic_SetPriority(IRQn_Type irqn, uint8 preempt, uint8 sub)
{
    /*
     * For STM32F4 (Cortex-M4), priority is in the upper 4 bits of the IP byte.
     * Assuming PRIGROUP = 3 (4 bits pre-emption, 0 bits sub-priority).
     */
    uint8 priority = (preempt << 4) | (sub & 0x0F);
    
    if ((sint32)irqn >= 0)
    {
        NVIC->IP[(uint32)irqn] = (uint8)(priority & 0xF0);
    }
}

/* ------------------------------------------------------------------ */
void Nvic_SetPriorityGrouping(uint32 grouping)
{
    uint32 reg_value;
    reg_value  =  SCB->AIRCR;
    reg_value &= ~((0xFFFFUL << 16) | (7UL << 8));
    reg_value |=  ((0x5FAUL << 16) | ((grouping & 7UL) << 8));
    SCB->AIRCR =  reg_value;
}
