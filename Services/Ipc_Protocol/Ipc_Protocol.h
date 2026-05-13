#ifndef IPC_PROTOCOL_H_
#define IPC_PROTOCOL_H_

#include "Elevator_Types.h"
#include "Std_Types.h"

/* Header value required by the protocol */
#define IPC_HEADER_VALUE 0xA5U

/* Build an 8-byte IPC frame from an ElevatorState_t */
void Ipc_BuildFrame(const ElevatorState_t* src, Ipc_SpiFrame_t* outFrame);

/* Verify checksum and header on a received frame. Returns 1 if valid, 0 otherwise. */
uint8 Ipc_VerifyFrame(const Ipc_SpiFrame_t* frame);

/* Compute checksum (XOR of bytes 0..6) */
uint8 Ipc_ComputeChecksum(const uint8* bytes, uint8 len);

#endif /* IPC_PROTOCOL_H_ */
