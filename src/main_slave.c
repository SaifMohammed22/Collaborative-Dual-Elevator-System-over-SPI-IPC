/* Enable slave firmware by default on this branch */
#include "Std_Types.h"

#define TURN_OFF_SLAVE 0U  /* Set to 1 to turn off, 0 to enable */

#include "Spi.h"
#include "Ipc_Protocol.h"
#include "Scheduler.h"
#include "Elevator_Types.h"
#include "Critical_Section.h"
#include "Timer.h"
#include "Elevator_Fsm.h"
#include "Push_Button.h"
#include "Elevator_Motor.h"
#include "Floor_Sensor.h"

extern ElevatorState_t MasterElevator;
extern ElevatorState_t SlaveElevator;

/* System Clock Frequency */
#define SYS_CLOCK_FREQ  16000000U /* 16 MHz HSI */

volatile uint32 g_SysTick_Ms = 0U;

void SysTick_Handler(void) {
    g_SysTick_Ms++;
}

int main(void) {
    #if TURN_OFF_SLAVE
        while (1) { }
    #else
        /* ---- Peripheral Initialisation ---- */
        Timer_Init_SysTick(SYS_CLOCK_FREQ);
        Spi_InitSlave();
        Scheduler_Init_50ms(SYS_CLOCK_FREQ);

        PushButton_Init();
        ElevatorMotor_Init();
        FloorSensor_Init();
        ElevatorFsm_Init();

        /* ---- Infinite loop ---- */
        while (1) {
            /* Run the FSM every iteration (non-blocking) */
            ElevatorFsm_Run();

            /* Periodic IPC exchange every 50ms */
            if (g_tick_50ms) {
                g_tick_50ms = 0U;

                /* Snapshot live FSM state into the SlaveElevator struct */
                ElevatorFsm_GetLiveState(&SlaveElevator);

                Ipc_SpiFrame_t txFrame;
                Ipc_SpiFrame_t rxFrame;

                Ipc_BuildFrame(&SlaveElevator, &txFrame);
                Spi_ExchangeFrame(&txFrame, &rxFrame);

                if (Ipc_VerifyFrame(&rxFrame)) {
                    ENTER_CRITICAL();
                    MasterElevator = rxFrame.data;
                    SlaveElevator.system_flags &= ~FLAG_COMM_FAULT;
                    EXIT_CRITICAL();
                } else {
                    ENTER_CRITICAL();
                    SlaveElevator.system_flags |= FLAG_COMM_FAULT;
                    EXIT_CRITICAL();
                }
            }
        }
    #endif
}