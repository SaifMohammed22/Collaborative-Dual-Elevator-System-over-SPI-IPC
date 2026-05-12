#include "Scheduler.h"
#include "Timer.h"
#include "Telemetry.h"

/* Shared elevator states (defined in App layer or IPC layer) */
extern ElevatorState_t MasterElevator;
extern ElevatorState_t SlaveElevator;

static void Scheduler_PeriodicTask(void) {
    /* Called exactly every 500ms (2Hz) from TIM3 ISR */
    Telemetry_SendState(&MasterElevator, &SlaveElevator);
}

void Scheduler_Init(void) {
    /* Initialize the timer for 500ms ticks */
    Timer3_Init();
    
    /* Link the callback to the Timer3 ISR */
    Timer3_SetCallback(Scheduler_PeriodicTask);
}
