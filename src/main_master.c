#include "Std_Types.h"
#include "Mcu_Hw.h"
#include "Bit_Operations.h"
#include "Timer.h"
#include "Uart.h"
#include "Telemetry.h"
#include "Elevator_Fsm.h"
#include "Elevator_Types.h"
#include "Hallway_Button.h"
#include "Debounce.h"

#define TEST_PHASE_1 1U /* Set to 1 to test, 0 to disable */
#define TURN_OFF_MASTER 0U /* Set to 1 to turn off, 0 to enable */

/* System Clock Frequency */
#define SYS_CLOCK_FREQ  16000000U /* 16 MHz HSI */
#define APB1_CLOCK_FREQ 16000000U

extern ElevatorState_t MasterElevator;

/* Define the global SysTick counter used by Elevator_Fsm.c */
volatile uint32 g_SysTick_Ms = 0U;

void SysTick_Handler(void) {
    g_SysTick_Ms++;
}

static void SysTick_Init(uint32 sys_clock_freq) {
    SysTick->LOAD = (sys_clock_freq / 1000U) - 1U;
    SysTick->VAL = 0U;
    /* CLKSOURCE = 1 (Processor clock), TICKINT = 1, ENABLE = 1 */
    SysTick->CTRL = (1U << 2U) | (1U << 1U) | (1U << 0U); 
}

#if TEST_PHASE_1
/* Dummy Data for Phase 1 Testing */
ElevatorState_t dummy_slave = {
    .current_floor = 1,
    .target_floor = 0,
    .state = ELEV_IDLE,
    .system_flags = FLAG_NORMAL
};
#endif

int main(void) {
    uint32 last_debounce_tick = 0U;

    /* 1. Initialize the RCC (System Clock, APB1/APB2 clocks, GPIOA clock) */
    /* Assuming default HSI 16MHz is already selected as System Clock by startup code. */

    /* 2. Call the UART, Timer, SysTick, and FSM initialization functions */
    Uart_Init_USART2_115200(APB1_CLOCK_FREQ);
    Timer_Init_TIM3_500ms(SYS_CLOCK_FREQ);
    SysTick_Init(SYS_CLOCK_FREQ);
    
    HAL_HallwayButton_Init();
    ElevatorFsm_Init();
    
    /* 3. Infinite loop */
    #if TURN_OFF_MASTER
        /* Safe, empty loop */
        while(1) {}

    #elif TEST_PHASE_1
        while (1) {
            /* Execute one tick of the FSM */
            ElevatorFsm_Run();
            
            /* 10ms Debounce Processing Tick */
            if ((g_SysTick_Ms - last_debounce_tick) >= 10U) {
                last_debounce_tick = g_SysTick_Ms;
                Debounce_Process();
            }
            
            /* Check the volatile 500ms flag */
            if (g_tick_500ms == 1U) {
                /* Clear the flag */
                g_tick_500ms = 0U;
                
                /* Fetch live FSM state into the telemetry struct */
                ElevatorFsm_GetLiveState(&MasterElevator);
                
                /* Call the Telemetry layer formatter */
                Telemetry_SendState(&MasterElevator, &dummy_slave);
            }
        }
    #else
        while (1) {
            /* Execute FSM normally without phase 1 telemetry test */
            ElevatorFsm_Run();
            
            /* 10ms Debounce Processing Tick */
            if ((g_SysTick_Ms - last_debounce_tick) >= 10U) {
                last_debounce_tick = g_SysTick_Ms;
                Debounce_Process();
            }
        }
    #endif

    return 0;
}
