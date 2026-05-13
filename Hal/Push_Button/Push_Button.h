#ifndef PUSH_BUTTON_H_
#define PUSH_BUTTON_H_

#include "Std_Types.h"

/* ------------------------------------------------------------------ */
/*  Button IDs                                                         */
/* ------------------------------------------------------------------ */
typedef enum {
    BTN_HALLWAY_U1 = 0U,
    BTN_HALLWAY_D2,
    BTN_HALLWAY_U2,
    BTN_HALLWAY_D3,
    BTN_HALLWAY_U3,
    BTN_HALLWAY_D4,
    BTN_CABIN_FLOOR1,
    BTN_CABIN_FLOOR2,
    BTN_CABIN_FLOOR3,
    BTN_CABIN_FLOOR4,
    BTN_EMERGENCY_STOP,
    BTN_COUNT                 /* sentinel — total number of buttons (11) */
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
 */
boolean PushButton_GetAndClear(PushButton_IdType id);

/**
 * @brief  Directly read the emergency-stop flag (volatile).
 * @return TRUE if the emergency stop is active.
 */
boolean PushButton_IsEmergencyActive(void);

/**
 * @brief  Clear the emergency-stop flag.
 */
void PushButton_ClearEmergency(void);

#endif /* PUSH_BUTTON_H_ */
