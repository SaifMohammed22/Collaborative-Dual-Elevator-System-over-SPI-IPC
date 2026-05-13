/**
 * @file    Floor_Sensor.h
 * @brief   HAL — Floor sensor abstraction (EXTI-driven).
 *
 * Each floor has a physical sensor (e.g. IR break-beam or hall-effect)
 * that triggers an EXTI interrupt when the elevator cabin arrives.
 * The HAL exposes only a "current floor" variable updated by the ISR.
 */

#ifndef FLOOR_SENSOR_H_
#define FLOOR_SENSOR_H_

#include "Std_Types.h"

/* ------------------------------------------------------------------ */
/*  Constants                                                          */
/* ------------------------------------------------------------------ */
#define FLOOR_UNKNOWN   0U    /* sentinel: position not yet determined */
#define FLOOR_MIN       1U
#define FLOOR_MAX       4U

/* ------------------------------------------------------------------ */
/*  API                                                                */
/* ------------------------------------------------------------------ */

/**
 * @brief  Initialise the 4 floor-sensor GPIO pins as EXTI inputs.
 */
void FloorSensor_Init(void);

/**
 * @brief  Getter for the last detected floor.
 * @return 1..4 corresponding to the floor number.
 */
uint8 FloorSensor_GetCurrentFloor(void);

/**
 * @brief  Setter for the current floor (called from EXTI ISR).
 * @param  floor_num The detected floor number (1..4).
 */
void FloorSensor_SetCurrentFloor(uint8 floor_num);

#endif /* FLOOR_SENSOR_H_ */
