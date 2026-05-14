# Collaborative-Dual-Elevator-System-over-SPI-IPC

## 🏢 Project Overview

This project implements a robust, real-time dual-elevator control system using two **STM32F401xE** microcontrollers. The system operates in a **Master-Slave configuration** communicating via a custom **SPI Inter-Processor Communication (IPC)** protocol.

The Master node is responsible for global task allocation (Dispatcher algorithm) and controlling Elevator A, while the Slave node controls Elevator B based on assignments received from the Master.

## ✨ Key Features

* **Layered Architecture:** Strictly adheres to standard embedded architecture principles (MCAL, HAL, Services, Application) using pure C and CMSIS register-level programming.
* **Smart Dispatching Algorithm:** The Master evaluates all incoming Hallway calls and routes them based on elevator state (Idle/Moving), direction, absolute distance, and communication health.
* **Zero-Overhead DMA Telemetry:** Uses DMA1 Stream 6 to transmit 500ms system state heartbeats over USART2 without burning CPU cycles.
* **Hardware Scheduler:** Timer-driven non-blocking scheduler (50ms for IPC, 500ms for Telemetry) optimized with WFI (Wait For Interrupt) for maximum power efficiency and simulator performance.
* **Fail-Safe IPC Mechanism:** Continuous SPI packet verification. If the Slave disconnects or packets corrupt, the Master automatically triggers `FLAG_COMM_FAULT` and routes all building calls to itself.

## 🗂️ Software Architecture

The codebase is cleanly decoupled into four main layers:

1. **Application Layer (`App/`)**:
    * `Dispatcher`: The global routing brain. Evaluates target floors and updates assignments.
    * `Elevator_Fsm`: The local Finite State Machine managing motor speed, door states, and floor transitions.
2. **Services Layer (`Services/`)**:
    * `Ipc_Protocol`: Formats, verifies, and parses SPI frames.
    * `Scheduler`: Hardware timer callbacks triggering periodic system tasks.
    * `Telemetry`: Formats global state data into human-readable strings for UART/DMA transmission.
3. **Hardware Abstraction Layer (`Hal/`)**:
    * `Elevator_Motor`: High-level PWM motor control.
    * `Floor_Sensor` / `Push_Button`: Debounced inputs for hallway and cabin interfaces.
4. **Microcontroller Abstraction Layer (`Mcal/`)**:
    * `Dma`, `Exti`, `Gpio`, `Nvic`, `Pwm`, `Spi`, `Timer`, `Uart`.

## 🛠️ Hardware & Pin Mapping (STM32F401xE)

* **Motor PWM (Master):** `PA0` (TIM2_CH1)
* **Telemetry UART:** `PA2` (USART2_TX)
* **SPI IPC:** Standard SPI pins between Master and Slave.
* **Hallway Inputs:** `PB0` - `PB5` (Configured with EXTI and internal Pull-ups).
* **Cabin Inputs:** Managed independently by respective Master/Slave nodes.

## 🚀 Build Instructions

This project uses **CMake** and the **ARM GNU Toolchain**.

1. Clone the repository.
2. Create a build directory: `mkdir build && cd build`
3. Configure with CMake: `cmake -DCMAKE_BUILD_TYPE=Debug ..`
4. Build the project: `cmake --build .`
5. The output binaries (`main_master.hex`, `main_slave.hex`, `main_master.elf`, `main_slave.elf`) will be generated in the `build` directory.

## 🖥️ Simulation (Proteus 8)

This project is designed to be fully simulated in Proteus 8 Professional.

### Simulator Quirks & Workarounds

Due to known limitations in the Proteus physics and logic rendering engines, specific workarounds are implemented in this codebase:

* **TIM2 Alternate Function Bug:** Proteus STM32F4 models often fail to route Alternate Functions to physical pins (e.g., TIM2_CH1 to PA0). A visual bypass using manual `GPIO_MODE_OUTPUT` toggling is implemented in `Elevator_Motor.c` to allow visual verification of the FSM states.
* **100% Duty Cycle Glitch:** A 100% PWM duty cycle (`CCR == ARR`) causes Proteus to render a flat 0V line. The codebase caps maximum duty at `ARR - 1` to ensure correct square wave rendering.
* **Time Dilation:** High-frequency SysTick interrupts (1ms) overload the simulator. The SysTick is decimated to 50ms, and `__asm volatile("wfi");` is used to sleep the CPU, keeping simulation time mapped accurately to real-time.

## 📜 License

This project is developed for educational purposes.
