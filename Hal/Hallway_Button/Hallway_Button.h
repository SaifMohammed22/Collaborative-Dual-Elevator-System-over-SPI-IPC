#ifndef HALLWAY_BUTTON_H_
#define HALLWAY_BUTTON_H_

#include "Std_Types.h"

/* 
 * 6 Hallway buttons to cover both systems.
 */
typedef enum {
    HALLWAY_UP_FLOOR1 = 0U,
    HALLWAY_UP_FLOOR2 = 1U,
    HALLWAY_DOWN_FLOOR2 = 2U,
    HALLWAY_UP_FLOOR3 = 3U,
    HALLWAY_DOWN_FLOOR3 = 4U,
    HALLWAY_DOWN_FLOOR4 = 5U
} HallwayButton_Id_t;

/**
 * @brief  Initializes GPIO PB0-PB5 and maps them to EXTI lines.
 */
void HAL_HallwayButton_Init(void);

/**
 * @brief  Safely returns and clears the raw EXTI flag for a specific button.
 *         Intended to be called by the Debounce Service.
 */
uint8 HAL_HallwayButton_GetRawFlag(HallwayButton_Id_t btnId);

#endif /* HALLWAY_BUTTON_H_ */
