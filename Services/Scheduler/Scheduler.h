#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include "Elevator_Types.h"
#include "Elevator_Fsm.h"

void Scheduler_SendTelemetry(ElevatorState_t* master_state, ElevatorState_t* slave_state);

#endif /* SCHEDULER_H_ */
