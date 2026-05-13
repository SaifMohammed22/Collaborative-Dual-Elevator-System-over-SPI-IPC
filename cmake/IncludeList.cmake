set(INCLUDE_LIST ${INCLUDE_LIST}
        ${ARM_DIR}/arm-none-eabi/include
        ${PROJECT_PATH}/STM32-base/startup
        ${PROJECT_PATH}/STM32-base-STM32Cube/CMSIS/ARM/inc
        ${PROJECT_PATH}/STM32-base-STM32Cube/CMSIS/${SERIES_FOLDER}/inc
        ${PROJECT_PATH}/include
        
        # --- Foundational Libraries ---
        ${PROJECT_PATH}/Lib

        # --- MCAL Layer ---
        ${PROJECT_PATH}/Mcal/Gpio
        ${PROJECT_PATH}/Mcal/Exti
        ${PROJECT_PATH}/Mcal/Nvic
        ${PROJECT_PATH}/Mcal/Pwm
        ${PROJECT_PATH}/Mcal/Rcc
        ${PROJECT_PATH}/Mcal/Timer
        ${PROJECT_PATH}/Mcal/Spi
        ${PROJECT_PATH}/Mcal/Uart
        ${PROJECT_PATH}/Mcal/Dma

        # --- HAL Layer ---
        ${PROJECT_PATH}/Hal/Cabin_Button
        ${PROJECT_PATH}/Hal/Hallway_Button
        ${PROJECT_PATH}/Hal/Floor_Sensor
        ${PROJECT_PATH}/Hal/Elevator_Motor

        # --- App Layer ---
        ${PROJECT_PATH}/App/Elevator_Fsm
        ${PROJECT_PATH}/App/Dispatcher

        # --- Services ---
        ${PROJECT_PATH}/Services/Ipc_Protocol
        ${PROJECT_PATH}/Services/Telemetry
        ${PROJECT_PATH}/Services/Debounce
)

if (USE_HAL)
    set(INCLUDE_LIST ${INCLUDE_LIST} ${PROJECT_PATH}/STM32-base-STM32Cube/HAL/${SERIES_FOLDER}/inc)
endif ()