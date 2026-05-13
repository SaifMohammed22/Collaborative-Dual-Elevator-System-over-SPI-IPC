#ifndef DEBOUNCE_H_
#define DEBOUNCE_H_

#include "Std_Types.h"
#include "Hallway_Button.h"

/**
 * @brief  Called periodically to evaluate raw EXTI flags and apply cooldown debounce.
 */
void Debounce_Process(void);

/**
 * @brief  Returns 1 if a debounced, valid press occurred for the given button, then clears it.
 */
uint8 Debounce_GetConfirmedCall(HallwayButton_Id_t btnId);

#endif /* DEBOUNCE_H_ */
