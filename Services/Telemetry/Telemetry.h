#ifndef TELEMETRY_H_
#define TELEMETRY_H_

#include "Elevator_Types.h"

/* Formats the shared ElevatorState_t into an internal buffer and triggers UART TX */
void Telemetry_SendState(const ElevatorState_t* masterState, const ElevatorState_t* slaveState);

#endif /* TELEMETRY_H_ */
