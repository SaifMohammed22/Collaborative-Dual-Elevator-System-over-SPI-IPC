#ifndef ELEVATOR_TYPES_H
#define ELEVATOR_TYPES_H

#include "Std_Types.h"
#include <stdint.h>

/* ------------------------------------------------------------------------- */
/* 1. Enumerations for Readability                                           */
/* ------------------------------------------------------------------------- */

/* Defines the exact operational state of the Finite State Machine */
typedef enum {
    ELEV_IDLE         = 0x00, /* Stopped, no pending targets */
    ELEV_MOVING_UP    = 0x01, /* Actively moving up (LED ramping/on) */
    ELEV_MOVING_DOWN  = 0x02, /* Actively moving down (LED ramping/on) */
    ELEV_DOORS_OPEN   = 0x03, /* Reached floor, doors are open */
    ELEV_FAULT        = 0x04  /* Critical error state */
} Elev_Fsm_State_t;

/* System Flags Bitmasks (Allows multiple active flags simultaneously) */
#define FLAG_NORMAL             (0x00)
#define FLAG_EMERGENCY_STOP     (1 << 0) /* Highest priority EXTI triggered */
#define FLAG_DOORS_BLOCKED      (1 << 1) /* Optional: if doors can't close */
#define FLAG_COMM_FAULT         (1 << 2) /* Master sets this if Slave times out */

/* ------------------------------------------------------------------------- */
/* 2. Core Elevator State Structure (The "Brain" / Application Layer)        */
/* ------------------------------------------------------------------------- */

/* * This is the struct you will use in Part 3 for your Dispatcher algorithm.
 * It strictly uses uint8_t to ensure easy 1-to-1 mapping into the SPI array.
 */
typedef struct {
    uint8_t current_floor;  /* Valid range: 1, 2, 3, or 4 */
    uint8_t target_floor;   /* Valid range: 1, 2, 3, 4 (0 means NO_TARGET) */
    Elev_Fsm_State_t state; /* Current physical FSM state */
    uint8_t system_flags;   /* Bitmask using the FLAG_ macros above */
} ElevatorState_t;


/* ------------------------------------------------------------------------- */
/* 3. IPC SPI Frame Structure (The "Bridge" / Part 2 Layer)                  */
/* ------------------------------------------------------------------------- */

/* * Tell Part 2 to use this __attribute__((packed)) struct. 
 * This guarantees the compiler will not add padding bytes, ensuring the 
 * struct is EXACTLY 8 bytes long for the physical SPI transmission.
 */
typedef struct __attribute__((packed)) {
    uint8_t header;         /* MUST be 0xA5 per the packet diagram */
    ElevatorState_t data;   /* The 4-byte application state defined above */
    uint8_t reserved_1;     /* Padding: 0x00 */
    uint8_t reserved_2;     /* Padding: 0x00 */
    uint8_t checksum;       /* XOR of bytes 0 through 6 */
} Ipc_SpiFrame_t;

#endif /* ELEVATOR_TYPES_H */
