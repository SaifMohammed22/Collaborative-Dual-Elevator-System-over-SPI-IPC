#ifndef DISPATCHER_H_
#define DISPATCHER_H_

#include "Std_Types.h"
#include "Elevator_Types.h"

typedef enum {
    DIR_UP = 0,
    DIR_DOWN = 1
} Direction_t;

/* Entry point for the task allocation algorithm */
void Dispatch_Call(uint8 requested_floor, Direction_t requested_dir);

#endif /* DISPATCHER_H_ */
