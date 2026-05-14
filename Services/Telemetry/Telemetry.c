#include "Telemetry.h"
#include "Uart.h"
#include "Critical_Section.h"

/* Double-buffered: DMA reads from one while we build the next */
static char telemetryBuffer[128];

static void IntToStr(uint8 num, char* str, uint8* len) {
    if (num == 0) {
        str[0] = '0';
        *len = 1;
        return;
    }
    uint8 temp = num;
    uint8 digits = 0;
    while (temp > 0) {
        digits++;
        temp /= 10;
    }
    *len = digits;
    for (uint8 i = 0; i < digits; i++) {
        str[digits - 1 - i] = (num % 10) + '0';
        num /= 10;
    }
}

static void HexToStr2(uint8 num, char* str) {
    uint8 high = num >> 4;
    uint8 low = num & 0x0F;
    str[0] = (high < 10) ? (high + '0') : (high - 10 + 'A');
    str[1] = (low < 10) ? (low + '0') : (low - 10 + 'A');
}

static void AppendStr(char* dest, uint8* destLen, const char* src) {
    while (*src) {
        dest[*destLen] = *src;
        (*destLen)++;
        src++;
    }
}

static void AppendInt(char* dest, uint8* destLen, uint8 val) {
    uint8 len = 0;
    IntToStr(val, &dest[*destLen], &len);
    *destLen += len;
}

static void AppendHex2(char* dest, uint8* destLen, uint8 val) {
    HexToStr2(val, &dest[*destLen]);
    *destLen += 2;
}

void Telemetry_SendState(const ElevatorState_t* masterState, const ElevatorState_t* slaveState) {
    ElevatorState_t localMaster;
    ElevatorState_t localSlave;
    uint8 idx = 0;

    if (masterState == NULL_PTR || slaveState == NULL_PTR) {
        return;
    }
    
    /* Atomically copy the structs to prevent tearing if they are updated by SPI ISR */
    ENTER_CRITICAL();
    localMaster = *masterState;
    localSlave = *slaveState;
    EXIT_CRITICAL();
    
    /* Safely format into buffer without sprintf */
    AppendStr(telemetryBuffer, &idx, "MASTER: Flr=");
    AppendInt(telemetryBuffer, &idx, localMaster.current_floor);
    AppendStr(telemetryBuffer, &idx, ", Tgt=");
    AppendInt(telemetryBuffer, &idx, localMaster.target_floor);
    AppendStr(telemetryBuffer, &idx, ", St=");
    AppendInt(telemetryBuffer, &idx, localMaster.state);
    AppendStr(telemetryBuffer, &idx, ", Flg=0x");
    AppendHex2(telemetryBuffer, &idx, localMaster.system_flags);
    
    AppendStr(telemetryBuffer, &idx, " | SLAVE: Flr=");
    AppendInt(telemetryBuffer, &idx, localSlave.current_floor);
    AppendStr(telemetryBuffer, &idx, ", Tgt=");
    AppendInt(telemetryBuffer, &idx, localSlave.target_floor);
    AppendStr(telemetryBuffer, &idx, ", St=");
    AppendInt(telemetryBuffer, &idx, localSlave.state);
    AppendStr(telemetryBuffer, &idx, ", Flg=0x");
    AppendHex2(telemetryBuffer, &idx, localSlave.system_flags);
    
    AppendStr(telemetryBuffer, &idx, "\r\n");
    telemetryBuffer[idx] = '\0';
            
    /* Use DMA for zero-CPU-overhead transmission */
    Uart_SendString_DMA(telemetryBuffer, (uint16)idx);
}
