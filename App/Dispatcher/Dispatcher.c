#include "Dispatcher.h"

/* Define the global state variables to be used by the App and Telemetry */
ElevatorState_t MasterElevator = {1, 0, ELEV_IDLE, FLAG_NORMAL};
ElevatorState_t SlaveElevator  = {1, 0, ELEV_IDLE, FLAG_NORMAL};

void Dispatch_Call(uint8_t requested_floor, Direction_t requested_dir) {
    /* Algorithm to be implemented in Phase 3 */
    (void)requested_floor;
    (void)requested_dir;
}
