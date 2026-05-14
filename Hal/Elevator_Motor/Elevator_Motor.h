/**
 * @file    Elevator_Motor.h
 * @brief   HAL — Motor abstraction for the elevator cabin.
 *
 * Simulates a motor via an LED driven by a PWM timer.
 * Exposes three speed presets (Stop / Slow / Full) and a
 * generic duty-cycle setter.
 *
 * The App layer includes ONLY this header — no MCAL details leak.
 */

#ifndef ELEVATOR_MOTOR_H_
#define ELEVATOR_MOTOR_H_

#include "Std_Types.h"

/* ------------------------------------------------------------------ */
/*  Speed presets (percentage of max duty cycle)                       */
/* ------------------------------------------------------------------ */
#define MOTOR_SPEED_STOP     0U      /* 0/10  — cabin stationary      */
#define MOTOR_SPEED_SLOW     2U      /* 2/10  — approaching target     */
#define MOTOR_SPEED_FULL    10U      /* 10/10 — full speed transit     */

/* ------------------------------------------------------------------ */
/*  API                                                                */
/* ------------------------------------------------------------------ */

/**
 * @brief  Initialise the motor PWM output (pin, timer, channel).
 *         The motor starts in the STOP (0 %) state.
 */
void ElevatorMotor_Init(void);

/**
 * @brief  Set the motor speed duty cycle (0-10).
 * @param  speed_pct  0 = stop, 2 = slow, 10 = full.
 */
void Elevator_SetSpeed(uint8 speed_pct);

/**
 * @brief  Software PWM tick function to be called every 1ms.
 */
void ElevatorMotor_SoftwarePwmTick(void);

#endif /* ELEVATOR_MOTOR_H_ */
