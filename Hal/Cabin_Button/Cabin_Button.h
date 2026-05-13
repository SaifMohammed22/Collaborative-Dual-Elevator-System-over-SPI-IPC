/**
 * @file    Push_Button.h
 * @brief   HAL — Push-button abstraction for cabin request and
 *          emergency-stop buttons (EXTI-driven, no polling).
 *
 * The HAL layer sees only its own header + Std_Types.
 * MCAL details (GPIO, EXTI, NVIC) are hidden in the .c file.
 */

#ifndef PUSH_BUTTON_H_
#define PUSH_BUTTON_H_

#include "Std_Types.h"

/* ------------------------------------------------------------------ */
/*  Button IDs                                                         */
/* ------------------------------------------------------------------ */
typedef enum {
    BTN_CABIN_FLOOR1 = 0U,
    BTN_CABIN_FLOOR2,
    BTN_CABIN_FLOOR3,
    BTN_CABIN_FLOOR4,
    BTN_EMERGENCY_STOP,
    BTN_COUNT                 /* sentinel — total number of buttons */
} PushButton_IdType;

/* ------------------------------------------------------------------ */
/*  API                                                                */
/* ------------------------------------------------------------------ */

/**
 * @brief  Initialise all button GPIOs as inputs with pull-up,
 *         configure EXTI lines with falling-edge trigger,
 *         and enable NVIC IRQs (emergency stop at highest priority).
 */
void PushButton_Init(void);

/**
 * @brief  Return and atomically clear the "pressed" flag for a button.
 * @param  id   Button identifier.
 * @return TRUE if the button was pressed since the last call, FALSE otherwise.
 *
 * Flags are set by the EXTI ISR and consumed here — no polling.
 */
uint8 PushButton_GetAndClear(PushButton_IdType id);

/**
 * @brief  Directly read the emergency-stop flag (volatile).
 * @return TRUE if the emergency stop is active.
 *
 * This is a separate fast-path for the FSM to check without
 * a generic loop over all buttons.
 */
uint8 PushButton_IsEmergencyActive(void);

/**
 * @brief  Clear the emergency-stop flag (called when the FSM
 *         has fully handled the stop condition).
 */
void PushButton_ClearEmergency(void);

#endif /* PUSH_BUTTON_H_ */
