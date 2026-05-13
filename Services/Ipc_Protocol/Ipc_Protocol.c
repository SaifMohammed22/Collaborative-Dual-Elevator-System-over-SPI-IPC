#include "Ipc_Protocol.h"
#include <string.h>

uint8 Ipc_ComputeChecksum(const uint8* bytes, uint8 len) {
    uint8 cs = 0U;
    for (uint8 i = 0; i < len; i++) {
        cs ^= bytes[i];
    }
    return cs;
}

void Ipc_BuildFrame(const ElevatorState_t* src, Ipc_SpiFrame_t* outFrame) {
    if (src == NULL || outFrame == NULL) return;

    outFrame->header = IPC_HEADER_VALUE;
    /* Copy 4 bytes of ElevatorState_t (packed as specified) */
    outFrame->data = *src;
    outFrame->reserved_1 = 0x00;
    outFrame->reserved_2 = 0x00;

    /* Compute checksum over bytes 0..6 (header through reserved_2) */
    uint8* bytes = (uint8*)outFrame;
    outFrame->checksum = Ipc_ComputeChecksum(bytes, 7);
}

uint8 Ipc_VerifyFrame(const Ipc_SpiFrame_t* frame) {
    if (frame == NULL) return 0U;
    if (frame->header != IPC_HEADER_VALUE) return 0U;
    const uint8* bytes = (const uint8*)frame;
    uint8 cs = Ipc_ComputeChecksum(bytes, 7);
    return (cs == frame->checksum) ? 1U : 0U;
}
