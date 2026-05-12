#include "Scheduler.h"
#include "Uart.h"
#include <stdio.h>

void Scheduler_SendTelemetry(ElevatorState_t* master_state, ElevatorState_t* slave_state) {
    char telemetryBuffer[128];
    
    if (master_state == NULL_PTR || slave_state == NULL_PTR) {
        return;
    }
    
    sprintf(telemetryBuffer, 
            "Elevator A: Floor %d State %d | Elevator B: Floor %d State %d\r\n",
            master_state->current_floor, master_state->state,
            slave_state->current_floor, slave_state->state);
            
    Uart_SendString(telemetryBuffer);
}
