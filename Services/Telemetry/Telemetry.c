#include "Telemetry.h"
#include "Uart.h"
#include "Critical_Section.h"
#include <stdio.h>

static char telemetryBuffer[128];

void Telemetry_SendState(const ElevatorState_t* masterState, const ElevatorState_t* slaveState) {
    ElevatorState_t localMaster;
    ElevatorState_t localSlave;

    if (masterState == NULL_PTR || slaveState == NULL_PTR) {
        return;
    }
    
    /* Atomically copy the structs to prevent tearing if they are updated by SPI ISR */
    ENTER_CRITICAL();
    localMaster = *masterState;
    localSlave = *slaveState;
    EXIT_CRITICAL();
    
    /* Safely format into buffer using sprintf */
    sprintf(telemetryBuffer, 
            "MASTER: Flr=%d, Tgt=%d, St=%d, Flg=0x%02X | SLAVE: Flr=%d, Tgt=%d, St=%d, Flg=0x%02X\r\n",
            localMaster.current_floor, localMaster.target_floor, localMaster.state, localMaster.system_flags,
            localSlave.current_floor, localSlave.target_floor, localSlave.state, localSlave.system_flags);
            
    Uart2_SendString(telemetryBuffer);
}
