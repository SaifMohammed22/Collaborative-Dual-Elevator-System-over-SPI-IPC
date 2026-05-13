/* Enable slave firmware by default on this branch */
#define TURN_OFF_SLAVE 0U  /* Set to 1 to turn off, 0 to enable */

#include "Spi.h"
#include "Ipc_Protocol.h"
#include "Scheduler.h"
#include "Elevator_Types.h"
#include "Critical_Section.h"

extern ElevatorState_t MasterElevator;
extern ElevatorState_t SlaveElevator;

int main(void) {
    #if TURN_OFF_SLAVE
        while (1) { }
    #else
        /* Initialize as a slave and respond to master's 50ms exchanges */
        Spi_InitSlave();
        Scheduler_Init_50ms(16000000U);

        while (1) {
            if (g_tick_50ms) {
                g_tick_50ms = 0U;

                Ipc_SpiFrame_t txFrame;
                Ipc_SpiFrame_t rxFrame;

                Ipc_BuildFrame(&SlaveElevator, &txFrame);
                Spi_ExchangeFrame(&txFrame, &rxFrame);

                if (Ipc_VerifyFrame(&rxFrame)) {
                    ENTER_CRITICAL();
                    MasterElevator = rxFrame.data;
                    EXIT_CRITICAL();
                }
            }
        }
    #endif
}