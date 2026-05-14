#include "Std_Types.h"
#include "Mcu_Hw.h"
#include "Bit_Operations.h"
#include "Timer.h"
#include "Uart.h"
#include "Telemetry.h"
#include "Elevator_Fsm.h"
#include "Elevator_Types.h"
#include "Spi.h"
#include "Ipc_Protocol.h"
#include "Scheduler.h"
#include "Critical_Section.h"
#include "Push_Button.h"
#include "Elevator_Motor.h"
#include "Floor_Sensor.h"
#include "Dispatcher.h"
#include "Dma.h"

/* System Clock Frequency */
#define SYS_CLOCK_FREQ  16000000U /* 16 MHz HSI */
#define APB1_CLOCK_FREQ 16000000U

extern ElevatorState_t MasterElevator;
extern ElevatorState_t SlaveElevator;

/* Define the global SysTick counter used by Elevator_Fsm.c */
volatile uint32 g_SysTick_Ms = 0U;

void SysTick_Handler(void) {
    g_SysTick_Ms++;
}

/* =================================================================== */
/*  Hallway Button → Dispatch_Call routing table                        */
/* =================================================================== */
typedef struct {
    PushButton_IdType btn;
    uint8             floor;
    Direction_t       dir;
} HallwayMapping_t;

static const HallwayMapping_t hallway_map[] = {
    { BTN_HALLWAY_U1, 1U, DIR_UP   },  /* PB0: Floor 1 UP   */
    { BTN_HALLWAY_D2, 2U, DIR_DOWN },  /* PB1: Floor 2 DOWN */
    { BTN_HALLWAY_U2, 2U, DIR_UP   },  /* PB2: Floor 2 UP   */
    { BTN_HALLWAY_D3, 3U, DIR_DOWN },  /* PB3: Floor 3 DOWN */
    { BTN_HALLWAY_U3, 3U, DIR_UP   },  /* PB4: Floor 3 UP   */
    { BTN_HALLWAY_D4, 4U, DIR_DOWN },  /* PB5: Floor 4 DOWN */
};
#define HALLWAY_MAP_SIZE (sizeof(hallway_map) / sizeof(hallway_map[0]))


int main(void) {
    /* 1. Initialize the RCC (System Clock, APB1/APB2 clocks, GPIOA clock) */
    /* Assuming default HSI 16MHz is already selected as System Clock by startup code. */

    /* 2. Call the UART, Timer, SysTick, and FSM initialization functions */
    Uart_Init_USART2_115200(APB1_CLOCK_FREQ);
    Timer_Init_TIM3_500ms(SYS_CLOCK_FREQ);
    Timer_Init_SysTick(SYS_CLOCK_FREQ);
    Spi_InitMaster();
    Scheduler_Init_50ms(SYS_CLOCK_FREQ);
    
    PushButton_Init();
    ElevatorMotor_Init();
    FloorSensor_Init();
    ElevatorFsm_Init();
    Dma_Uart2Tx_Init();
    
    /* 3. Infinite loop */
    while (1) {
        /* ---- Execute FSM (non-blocking) ---- */
        ElevatorFsm_Run();

        /* ---- Poll Hallway Buttons & Route to Dispatcher ---- */
        for (uint8 i = 0U; i < HALLWAY_MAP_SIZE; i++) {
            if (PushButton_GetAndClear(hallway_map[i].btn)) {
                Dispatch_Call(hallway_map[i].floor, hallway_map[i].dir);
            }
        }

        /* ---- Periodic IPC exchange every 50ms ---- */
        if (g_tick_50ms) {
            g_tick_50ms = 0U;

            /* Snapshot live Master FSM state before transmitting */
            ElevatorFsm_GetLiveState(&MasterElevator);

            Ipc_SpiFrame_t txFrame;
            Ipc_SpiFrame_t rxFrame;

            Ipc_BuildFrame(&MasterElevator, &txFrame);

            /* Inject pending Slave command into the reserved_1 byte */
            if (Dispatcher_SlaveCommand != 0U) {
                txFrame.reserved_1 = Dispatcher_SlaveCommand;
                Dispatcher_SlaveCommand = 0U;
                /* Recalculate checksum since we modified the frame */
                txFrame.checksum = Ipc_ComputeChecksum((const uint8*)&txFrame, 7);
            }

            Spi_ExchangeFrame(&txFrame, &rxFrame);

            if (Ipc_VerifyFrame(&rxFrame)) {
                ENTER_CRITICAL();
                SlaveElevator = rxFrame.data;
                MasterElevator.system_flags &= ~FLAG_COMM_FAULT; /* clear on success */
                EXIT_CRITICAL();
            } else {
                MasterElevator.system_flags |= FLAG_COMM_FAULT;
            }
        }

        /* ---- 500ms UART Telemetry ---- */
        if (g_tick_500ms) {
            g_tick_500ms = FALSE;
            Telemetry_SendState(&MasterElevator, &SlaveElevator);
        }
    }

    return 0;
}
