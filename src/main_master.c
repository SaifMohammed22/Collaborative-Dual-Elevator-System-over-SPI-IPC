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
    
    /* 3. Infinite loop */
    while (1) {
        /* Execute FSM normally */
        ElevatorFsm_Run();

        /* Periodic IPC exchange every 50ms */
        if (g_tick_50ms) {
            g_tick_50ms = 0U;
            Ipc_SpiFrame_t txFrame;
            Ipc_SpiFrame_t rxFrame;

            Ipc_BuildFrame(&MasterElevator, &txFrame);
            Spi_ExchangeFrame(&txFrame, &rxFrame);

            if (Ipc_VerifyFrame(&rxFrame)) {
                ENTER_CRITICAL();
                SlaveElevator = rxFrame.data;
                EXIT_CRITICAL();
            } else {
                MasterElevator.system_flags |= FLAG_COMM_FAULT;
            }
        }
    }

    return 0;
}
