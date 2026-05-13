/**
 * @file    Elevator_Fsm.c
 * @brief   APP — Elevator FSM implementation.
 *
 * ┌──────────────────────────────────────────────────────────────────┐
 * │  State Machine                                                   │
 * │                                                                  │
 * │                  ┌──────────┐                                    │
 * │         ┌───────►│   IDLE   │◄────────────────┐                  │
 * │         │        └────┬─────┘                  │                 │
 * │         │             │ Target != Current      │ door timer      │
 * │         │             ▼                        │ expired         │
 * │         │  ┌─────────────────────┐    ┌───────┴──────┐          │
 * │         │  │ MOVING_UP / _DOWN   │───►│ DOORS_OPEN   │          │
 * │         │  └─────────────────────┘    └──────────────┘          │
 * │         │     (speed: FULL→SLOW→STOP)                           │
 * │         │                                                        │
 * │         │  Emergency pressed at ANY time                         │
 * │         │  ────────────────────────►  EMERGENCY (motor = STOP)   │
 * │         │                             │                          │
 * │         └─────────────────────────────┘  (manual reset)          │
 * └──────────────────────────────────────────────────────────────────┘
 *
 *  Non-blocking approach:
 *      – The DOORS_OPEN state uses a SysTick-derived tick counter
 *        instead of HAL_Delay().  The counter is sampled on each
 *        ElevatorFsm_Run() invocation.
 *      – The "approach zone" (1 floor away) switches to SLOW speed
 *        based on floor-sensor feedback, not a timed delay.
 */

#include "Elevator_Fsm.h"

/* ---- HAL includes ONLY (no MCAL headers) -------------------------- */
#include "Elevator_Motor.h"
#include "Floor_Sensor.h"
#include "Cabin_Button.h"

/* =================================================================== */
/*  Global target variable (definition — declared extern in header)     */
/* =================================================================== */
volatile uint8 Target_Floor = 0U;

/* =================================================================== */
/*  Non-blocking tick counter (driven by SysTick @ 1 ms)               */
/* =================================================================== */

/**
 * @brief  Millisecond counter incremented by the SysTick ISR.
 *         Declared volatile since it is written in ISR context.
 */
extern volatile uint32 g_SysTick_Ms;

/**
 * @brief  Return the current millisecond tick count.
 */
static inline uint32 Tick_GetMs(void) { return g_SysTick_Ms; }

/**
 * @brief  Return TRUE if `duration_ms` has elapsed since `start_ms`.
 *         Handles 32-bit wrap-around correctly.
 */
static inline uint8 Tick_Elapsed(uint32 start_ms, uint32 duration_ms) {
  return ((Tick_GetMs() - start_ms) >= duration_ms) ? TRUE : FALSE;
}

/* =================================================================== */
/*  Timing constants                                                   */
/* =================================================================== */
#define DOOR_OPEN_DURATION_MS 3000U /* keep doors open for 3 s     */

/* =================================================================== */
/*  Private state                                                      */
/* =================================================================== */
static ElevatorFsm_StateType g_State;
static uint32 g_DoorOpenTimestamp;

/* =================================================================== */
/*  Initialisation                                                     */
/* =================================================================== */
void ElevatorFsm_Init(void) {
  g_State = FSM_STATE_IDLE;
  g_DoorOpenTimestamp = 0U;
  Target_Floor = 0U;
  Elevator_SetSpeed(MOTOR_SPEED_STOP);
}

/* =================================================================== */
/*  One FSM tick (non-blocking)                                        */
/* =================================================================== */
void ElevatorFsm_Run(void) {
  /* ----- Global pre-emption: Emergency Stop ---------------------- */
  if (PushButton_IsEmergencyActive()) {
    Elevator_SetSpeed(MOTOR_SPEED_STOP);
    g_State = FSM_STATE_EMERGENCY;
    /* Remain in EMERGENCY until the flag is externally cleared
     * (e.g. by the dispatcher or a physical reset).              */
    return;
  }

  uint8 current = FloorSensor_GetCurrentFloor();

  switch (g_State) {

  /* ============================================================== */
  case FSM_STATE_IDLE:
    /* ============================================================== */
    {
      /* ===================================================================
       * INTEGRATION WARNING (For Part 2 & 3)
       * ===================================================================
       * 1. In standalone mode, the FSM consumes button presses directly
       *    for local testing.
       * 2. During full system integration, this loop must be commented out
       *    (or STANDALONE_TESTING_MODE undefined).
       * 3. The SPI Bridge (Part 2) must be the one to read
       *    PushButton_GetAndClear(), transmit it to the Dispatcher (Part 3),
       *    and the Dispatcher will then write the final decision to
       *    Target_Floor.
       * =================================================================== */
#ifdef STANDALONE_TESTING_MODE
      /* Consume any pending cabin-button presses → set Target_Floor */
      for (uint8 f = 0U; f < 4U; f++) {
        if (PushButton_GetAndClear((PushButton_IdType)f)) {
          Target_Floor = f + 1U; /* buttons are 0-indexed */
        }
      }
#endif /* STANDALONE_TESTING_MODE */

      /* Nothing to do if target is unknown or already at target */
      if ((Target_Floor == 0U) || (Target_Floor == current)) {
        Target_Floor = 0U; /* clear consumed target */
        break;
      }

      /* Decide direction */
      if (Target_Floor > current) {
        g_State = FSM_STATE_MOVING_UP;
      } else {
        g_State = FSM_STATE_MOVING_DOWN;
      }

      /* Choose initial speed */
      if (Target_Floor == current + 1U || Target_Floor == current - 1U) {
        Elevator_SetSpeed(MOTOR_SPEED_SLOW); /* only 1 floor away */
      } else {
        Elevator_SetSpeed(MOTOR_SPEED_FULL); /* far away — full   */
      }
      break;
    }

  /* ============================================================== */
  case FSM_STATE_MOVING_UP:
    /* ============================================================== */
    {
      if ((Target_Floor == 0U) || (Target_Floor < current)) {
        Elevator_SetSpeed(MOTOR_SPEED_STOP);
        g_State = FSM_STATE_IDLE;
        break;
      }

      if (current == Target_Floor) {
        /* Arrived */
        Elevator_SetSpeed(MOTOR_SPEED_STOP);
        g_DoorOpenTimestamp = Tick_GetMs();
        g_State = FSM_STATE_DOORS_OPEN;
      } else if (current + 1U == Target_Floor) {
        /* One floor away — decelerate */
        Elevator_SetSpeed(MOTOR_SPEED_SLOW);
      }
      /* else: maintain full speed */
      break;
    }

  /* ============================================================== */
  case FSM_STATE_MOVING_DOWN:
    /* ============================================================== */
    {
      if ((Target_Floor == 0U) || (Target_Floor > current)) {
        Elevator_SetSpeed(MOTOR_SPEED_STOP);
        g_State = FSM_STATE_IDLE;
        break;
      }

      if (current == Target_Floor) {
        Elevator_SetSpeed(MOTOR_SPEED_STOP);
        g_DoorOpenTimestamp = Tick_GetMs();
        g_State = FSM_STATE_DOORS_OPEN;
      } else if (current - 1U == Target_Floor) {
        Elevator_SetSpeed(MOTOR_SPEED_SLOW);
      }
      break;
    }

  /* ============================================================== */
  case FSM_STATE_DOORS_OPEN:
    /* ============================================================== */
    {
      /*
       * Non-blocking door timer:
       * We sampled Tick_GetMs() when entering this state.
       * Each Run() call simply checks whether the duration has elapsed.
       */
      if (Tick_Elapsed(g_DoorOpenTimestamp, DOOR_OPEN_DURATION_MS)) {
        Target_Floor = 0U; /* clear — ready for next request */
        g_State = FSM_STATE_IDLE;
      }
      break;
    }

  /* ============================================================== */
  case FSM_STATE_EMERGENCY:
    /* ============================================================== */
    {
      /* Motor stays stopped.  Remain here until the emergency flag
       * is explicitly cleared by the upper layer or reset button.  */
      Elevator_SetSpeed(MOTOR_SPEED_STOP);

      if (!PushButton_IsEmergencyActive()) {
        /* Emergency resolved — return to IDLE */
        g_State = FSM_STATE_IDLE;
      }
      break;
    }

  default:
    /* Defensive: unknown state → IDLE + stop */
    Elevator_SetSpeed(MOTOR_SPEED_STOP);
    g_State = FSM_STATE_IDLE;
    break;
  }
}

/* =================================================================== */
/*  State query                                                        */
/* =================================================================== */
ElevatorFsm_StateType ElevatorFsm_GetState(void) { return g_State; }

void ElevatorFsm_GetLiveState(ElevatorState_t* out_state) {
    if (out_state == NULL_PTR) {
        return;
    }
    
    out_state->current_floor = FloorSensor_GetCurrentFloor();
    out_state->target_floor = Target_Floor;
    out_state->state = (Elev_Fsm_State_t)g_State;
    
    out_state->system_flags = FLAG_NORMAL;
    if (PushButton_IsEmergencyActive()) {
        out_state->system_flags |= FLAG_EMERGENCY_STOP;
    }
}
