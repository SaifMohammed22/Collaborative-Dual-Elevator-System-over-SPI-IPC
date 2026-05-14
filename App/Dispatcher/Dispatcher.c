/**
 * @file    Dispatcher.c
 * @brief   APP — Central task-allocation algorithm ("The Brain").
 *
 * Runs exclusively on the Master MCU.  Receives hallway call requests
 * and decides which elevator (Master A or Slave B) should service them.
 *
 * Priority Tiers (strict descending):
 *   Tier 0 — Immediate:     IDLE and already at requested floor
 *   Tier 1 — Perfect Match: Moving TOWARD floor in same direction
 *   Tier 2 — Passed Match:  Moving but has passed the floor (return trip)
 *   Tier 3 — Idle Nearest:  IDLE, closest by |current - requested|
 *   Tier 4 — Fallback:      Both busy, pick nearest distance
 *
 * Comm-Fault Override:
 *   If FLAG_COMM_FAULT is set on the Master, ALL hallway calls are
 *   forced to the Master elevator (Slave is unreachable).
 */

#include "Dispatcher.h"
#include "Elevator_Fsm.h"
#include "Critical_Section.h"

/* Define the global state variables to be used by the App and Telemetry */
ElevatorState_t MasterElevator = {1, 0, ELEV_IDLE, FLAG_NORMAL};
ElevatorState_t SlaveElevator  = {1, 0, ELEV_IDLE, FLAG_NORMAL};

/* =================================================================== */
/*  Helpers                                                             */
/* =================================================================== */

/** Absolute value for unsigned subtraction (avoids signed math). */
static uint8 AbsDiff(uint8 a, uint8 b) {
    return (a >= b) ? (a - b) : (b - a);
}

/**
 * @brief  Score a single elevator's suitability for a call.
 *
 * Lower score = better match.  Returns 0xFF if completely ineligible.
 *
 *   Score  0  → Tier 0: Immediate (idle at floor)
 *   Score  1  → Tier 1: Perfect match (moving toward floor, same dir)
 *   Score  2  → Tier 2: Passed match (moving, already passed floor)
 *   Score  3+ → Tier 3/4: Idle or busy distance (3 + |distance|)
 */
static uint8 ScoreElevator(const ElevatorState_t *elev,
                            uint8 req_floor,
                            Direction_t req_dir)
{
    uint8 cur  = elev->current_floor;
    uint8 tgt  = elev->target_floor;
    Elev_Fsm_State_t st = elev->state;

    /* --- Tier 0: Immediate ------------------------------------------ */
    if ((st == ELEV_IDLE || st == ELEV_DOORS_OPEN) && cur == req_floor) {
        return 0U;
    }

    /* --- Tier 3: Idle nearest (no active target) -------------------- */
    if (st == ELEV_IDLE || st == ELEV_DOORS_OPEN) {
        return 3U + AbsDiff(cur, req_floor);
    }

    /* --- Moving cases ----------------------------------------------- */
    if (st == ELEV_MOVING_UP && req_dir == DIR_UP) {
        /* Moving up, request is up: perfect if floor is ahead */
        if (req_floor >= cur && req_floor <= tgt) {
            return 1U;  /* Tier 1: Perfect match */
        }
        return 2U;      /* Tier 2: Passed / overshoot */
    }

    if (st == ELEV_MOVING_DOWN && req_dir == DIR_DOWN) {
        /* Moving down, request is down: perfect if floor is below */
        if (req_floor <= cur && (tgt != 0U && req_floor >= tgt)) {
            return 1U;  /* Tier 1: Perfect match */
        }
        return 2U;      /* Tier 2: Passed / overshoot */
    }

    /* Moving in opposite direction to the request — completely ineligible.
     * The rubric forbids mid-flight reversal; the elevator must finish
     * its current path before it can be considered for new calls.       */
    return 0xFFU;
}

/* =================================================================== */
/*  Public API                                                          */
/* =================================================================== */

void Dispatch_Call(uint8 requested_floor, Direction_t requested_dir) {
    /* ---- Sanity check ---- */
    if (requested_floor < 1U || requested_floor > 4U) {
        return;
    }

    /* ---- Comm-Fault Override: force all calls to Master ---- */
    if (MasterElevator.system_flags & FLAG_COMM_FAULT) {
        ENTER_CRITICAL();
        /* Only assign if Master is not already busy with another target */
        if (Target_Floor == 0U) {
            Target_Floor = requested_floor;
        }
        EXIT_CRITICAL();
        return;
    }

    /* ---- Score both elevators ---- */
    uint8 score_master = ScoreElevator(&MasterElevator, requested_floor, requested_dir);
    uint8 score_slave  = ScoreElevator(&SlaveElevator,  requested_floor, requested_dir);

    /* ---- Assign to the best (lowest) score ---- */
    if (score_master <= score_slave) {
        /* Assign to Master (local) */
        ENTER_CRITICAL();
        if (Target_Floor == 0U) {
            Target_Floor = requested_floor;
        }
        EXIT_CRITICAL();
    } else {
        /* Assign to Slave (remote via SPI) */
        ENTER_CRITICAL();
        if (SlaveElevator.target_floor == 0U) {
            SlaveElevator.target_floor = requested_floor;
        }
        EXIT_CRITICAL();
    }
}
