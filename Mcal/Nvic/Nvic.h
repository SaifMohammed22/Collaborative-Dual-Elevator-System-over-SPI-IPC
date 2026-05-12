/**
 * @file    Nvic.h
 * @brief   MCAL — NVIC configuration API for STM32F401xE.
 */

#ifndef NVIC_H_
#define NVIC_H_

#include "Std_Types.h"
#include "Mcu_Hw.h"

/* ------------------------------------------------------------------ */
/*  API                                                                */
/* ------------------------------------------------------------------ */

/**
 * @brief Enable an interrupt in the NVIC.
 * @param irqn  IRQ number (from CMSIS IRQn_Type).
 */
void Nvic_EnableIrq(IRQn_Type irqn);

/**
 * @brief Disable an interrupt in the NVIC.
 */
void Nvic_DisableIrq(IRQn_Type irqn);

/**
 * @brief Set the priority of an interrupt.
 * @param irqn      IRQ number.
 * @param preempt   Pre-emption priority (0 = highest).
 * @param sub       Sub-priority.
 *
 * Uses a fixed priority grouping of 4 bits pre-emption / 0 bits sub.
 */
void Nvic_SetPriority(IRQn_Type irqn, uint8_t preempt, uint8_t sub);

/**
 * @brief Configure the priority grouping (AIRCR.PRIGROUP).
 * @param grouping  Value 0 – 7 as per ARM Cortex-M specification.
 *                  3 => 4 bits pre-emption / 0 bits sub (recommended).
 */
void Nvic_SetPriorityGrouping(uint32_t grouping);

#endif /* NVIC_H_ */
