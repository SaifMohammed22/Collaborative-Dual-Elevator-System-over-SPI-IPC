/**
 * @file    main_slave_sandbox.c
 * @brief   Temporary test harness for Part 1 — Local Elevator Node.
 *
 * This sandbox:
 *   1. Initialises all HAL modules (buttons, sensors, motor).
 *   2. Configures SysTick for a 1 ms non-blocking tick.
 *   3. Hardcodes  Target_Floor = 4  on first cabin-button press.
 *   4. Runs the elevator FSM in a non-blocking super-loop.
 *
 * You can observe the LED on PA6 ramp up (100 % duty → 20 % → 0 %)
 * as the FSM reacts to floor-sensor EXTI edges.
 *
 * NOTE: This file is intended to be compiled *instead of* main_slave.c.
 *       Update cmake/SourceList.cmake accordingly or temporarily rename.
 *
 * @attention
 *   – `SystemClock_Config()` is NOT provided here.
 *     The default HSI @ 16 MHz after reset is assumed.
 *   – Replace `SystemCoreClock` with your actual value if you
 *     configure PLL elsewhere.
 */

#include "Elevator_Fsm.h"
#include "Elevator_Motor.h"
#include "Floor_Sensor.h"
#include "Push_Button.h"

#include "stm32f4xx.h"    /* SysTick, CMSIS core */

/* =================================================================== */
/*  Global SysTick counter (referenced by Elevator_Fsm.c)             */
/* =================================================================== */
volatile uint32_t g_SysTick_Ms = 0U;

/**
 * @brief  SysTick ISR — fires every 1 ms.
 */
void SysTick_Handler(void)
{
    g_SysTick_Ms++;
}

/* =================================================================== */
/*  SysTick init  (1 ms tick using CMSIS intrinsic)                    */
/* =================================================================== */
static void SysTick_Init(void)
{
    /*
     * SysTick_Config() loads LOAD = (ticks - 1), enables the counter
     * with the processor clock, and enables the SysTick_IRQn.
     *
     * SystemCoreClock is typically set by SystemInit() / startup code.
     * For default HSI on STM32F401:  SystemCoreClock = 16 000 000.
     */
    SysTick_Config(SystemCoreClock / 1000U);   /* 1 ms period */
}

/* =================================================================== */
/*  Main                                                               */
/* =================================================================== */
int main(void)
{
    /* ---- 0. Non-blocking tick timer ------------------------------- */
    SysTick_Init();

    /* ---- 1. Initialise HAL modules -------------------------------- */
    PushButton_Init();      /* cabin buttons + emergency stop         */
    FloorSensor_Init();     /* 4 floor-position sensors               */
    ElevatorMotor_Init();   /* PWM on PA6 / TIM3-CH1                  */

    /* ---- 2. Initialise the FSM ----------------------------------- */
    ElevatorFsm_Init();

    /*
     * ---- 3. Sandbox override: hardcode Target_Floor = 4 -----------
     *
     * In stand-alone mode we simply set the target once so you can
     * observe the LED ramp-up when you trigger the floor sensors
     * (PB5 → PB8) manually.
     *
     * For interactive testing, comment out the line below and
     * press cabin button 4 (PB0) instead — the EXTI ISR will
     * set Target_Floor = 4 through the FSM's button-polling logic.
     */
    Target_Floor = 4U;

    /* ---- 4. Super-loop (non-blocking) ----------------------------- */
    for (;;) {
        ElevatorFsm_Run();

        /*
         * Optional: enter low-power WFI between ticks.
         * The MCU will wake on any EXTI or SysTick interrupt.
         */
        __WFI();
    }

    /* unreachable */
    return 0;
}
