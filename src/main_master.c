#include "Std_Types.h"
#include "Mcu_Hw.h"
#include "Bit_Operations.h"
#include "Nvic.h"
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
    ElevatorMotor_SoftwarePwmTick();
}

/* Local TIM4 500ms periodic timer for Telemetry */
volatile uint8 g_tick_500ms = 0U;

void TIM4_IRQHandler(void) {
    if (GET_BIT(TIM4->SR, TIM_SR_UIF_Pos)) {
        CLR_BIT(TIM4->SR, TIM_SR_UIF_Pos);
        g_tick_500ms = 1U;
    }
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

    /* 2. Call the UART, SysTick, and FSM initialization functions */
    Uart_Init_USART2_115200(APB1_CLOCK_FREQ);
    
    /* ---- SysTick 1ms periodic timer ---- */
    SysTick->LOAD = (SYS_CLOCK_FREQ / 1000U) - 1U;
    SysTick->VAL  = 0U;
    SysTick->CTRL = (1U << 2U) | (1U << 1U) | (1U << 0U); /* CLKSOURCE=CPU, TICKINT=1, ENABLE=1 */

    /* ---- TIM4 500ms periodic timer for telemetry ---- */
    SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM4EN_Pos);
    TIM4->PSC = (SYS_CLOCK_FREQ / 10000U) - 1U; /* 0.1ms tick */
    TIM4->ARR = 5000U - 1U;                     /* 500ms */
    SET_BIT(TIM4->DIER, TIM_DIER_UIE_Pos);
    Nvic_EnableIrq(TIM4_IRQn);
    Nvic_SetPriority(TIM4_IRQn, 4U, 0U);
    SET_BIT(TIM4->CR1, TIM_CR1_CEN_Pos);
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

        /* ---- DIAGNOSTIC: Bypass TIM3 — blast telemetry with SW delay ---- */
        Telemetry_SendState(&MasterElevator, &SlaveElevator);
        for (volatile int i = 0; i < 500000; i++) { /* dirty ~500ms delay */ }
    }

    return 0;
}
