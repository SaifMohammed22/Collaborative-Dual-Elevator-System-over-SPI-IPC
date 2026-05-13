/**
 * @file    Exti.h
 * @brief   MCAL — EXTI driver for STM32F401xE.
 *
 * Configures external interrupt lines with trigger edge and SYSCFG
 * port-to-EXTI mapping.
 */

#ifndef EXTI_H_
#define EXTI_H_

#include "Std_Types.h"
#include "Mcu_Hw.h"

/* ------------------------------------------------------------------ */
/*  Trigger edge                                                       */
/* ------------------------------------------------------------------ */
typedef enum {
    EXTI_TRIGGER_RISING  = 0U,
    EXTI_TRIGGER_FALLING = 1U,
    EXTI_TRIGGER_BOTH    = 2U
} Exti_TriggerType;

/* ------------------------------------------------------------------ */
/*  Port index (maps to SYSCFG_EXTICRx)                                */
/* ------------------------------------------------------------------ */
typedef enum {
    EXTI_PORT_A = 0U,
    EXTI_PORT_B = 1U,
    EXTI_PORT_C = 2U,
    EXTI_PORT_D = 3U,
    EXTI_PORT_E = 4U,
    EXTI_PORT_H = 7U
} Exti_PortType;

/* ------------------------------------------------------------------ */
/*  Configuration aggregate                                            */
/* ------------------------------------------------------------------ */
typedef struct {
    uint8            Line;       /* EXTI line 0 – 15 (matches pin number) */
    Exti_PortType      Port;       /* Port that sources this line           */
    Exti_TriggerType   Trigger;
} Exti_CfgType;

/* ------------------------------------------------------------------ */
/*  API                                                                */
/* ------------------------------------------------------------------ */

/**
 * @brief Enable the SYSCFG peripheral clock (required before mapping).
 */
void Exti_EnableSysCfgClock(void);

/**
 * @brief Configure an EXTI line: map port, set trigger, unmask.
 */
void Exti_ConfigLine(const Exti_CfgType *cfg);

/**
 * @brief Clear the pending flag for the given EXTI line.
 *        Must be called inside the ISR before returning.
 */
void Exti_ClearPending(uint8 line);

/**
 * @brief  Check if the pending bit for a specific EXTI line is set.
 * @param  line   The EXTI line number (0..15).
 * @return TRUE if the pending bit is set, FALSE otherwise.
 */
boolean Exti_IsPending(uint8 line);

#endif /* EXTI_H_ */
