/**
 * @file    Elevator_Fsm.h
 * @brief   APP — Elevator finite state machine.
 *
 * The FSM is a "dumb" node: it reads the global `Target_Floor`,
 * compares it to the current floor (from floor sensors), and
 * drives the motor accordingly.  It knows nothing about the master
 * dispatcher or the SPI IPC layer.
 *
 * States: IDLE → MOVING_UP / MOVING_DOWN → DOORS_OPEN → IDLE
 *
 * The FSM is driven by calling `ElevatorFsm_Run()` from a
 * super-loop or a periodic scheduler.  It is entirely non-blocking:
 * no `while(1)` busy-waits, no `HAL_Delay()`.
 */

#ifndef ELEVATOR_FSM_H_
#define ELEVATOR_FSM_H_

#include "Std_Types.h"

/* ------------------------------------------------------------------ */
/*  FSM states (public so the scheduler / debug can inspect)           */
/* ------------------------------------------------------------------ */
typedef enum {
    FSM_STATE_IDLE        = 0U,
    FSM_STATE_MOVING_UP   = 1U,
    FSM_STATE_MOVING_DOWN = 2U,
    FSM_STATE_DOORS_OPEN  = 3U,
    FSM_STATE_EMERGENCY   = 4U
} ElevatorFsm_StateType;

/* ------------------------------------------------------------------ */
/*  Global target (written by dispatcher / buttons / sandbox)          */
/* ------------------------------------------------------------------ */

/**
 * @brief  The single shared variable that drives the FSM.
 *         Written by the master dispatcher (or locally by cabin
 *         buttons in stand-alone mode).  Read by the FSM.
 *
 *         MUST be `volatile` — it is written from ISR context
 *         (cabin-button EXTI) and read from the main loop.
 */
extern volatile uint8_t Target_Floor;

/* ------------------------------------------------------------------ */
/*  API                                                                */
/* ------------------------------------------------------------------ */

/**
 * @brief  Initialise the FSM to IDLE state and clear internal timers.
 */
void ElevatorFsm_Init(void);

/**
 * @brief  Execute one non-blocking tick of the FSM.
 *
 * Call this function from the super-loop or a periodic scheduler.
 * Each invocation evaluates current state, sensor inputs, and
 * Target_Floor, then transitions and actuates as needed.
 *
 * The function returns immediately — it never blocks.
 */
void ElevatorFsm_Run(void);

/**
 * @brief  Return the current FSM state (for debug / telemetry).
 */
ElevatorFsm_StateType ElevatorFsm_GetState(void);

#endif /* ELEVATOR_FSM_H_ */
