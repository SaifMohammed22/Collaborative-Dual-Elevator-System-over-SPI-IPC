#include "Uart.h"
#include "Mcu_Hw.h"
#include "Bit_Operations.h"
#include "Gpio.h"

void Uart_Init_USART2_115200(uint32 pclk_freq) {
    /* 1. Enable USART2 Clock (APB1, Bit 17) */
    SET_BIT(RCC->APB1ENR, 17U); 
    
    /* 2. Configure PA2 as Alternate Function for USART2 TX */
    Gpio_EnableClock(GPIOA);
    
    Gpio_CfgType txPinCfg = {
        .Port = GPIOA,
        .Pin = 2U,
        .Mode = GPIO_MODE_AF,
        .OType = GPIO_OTYPE_PUSHPULL,
        .PuPd = GPIO_PUPD_NONE,
        .Speed = GPIO_SPEED_HIGH,
        .AltFunc = 7U /* AF7 for USART2 TX */
    };
    Gpio_ConfigPin(&txPinCfg);
    
    /* 3. Configure Baud Rate */
    USART2->BRR = pclk_freq / 115200U;
    
    /* 4. Enable USART (UE=1), Transmitter (TE=1) */
    /* CR1: TE is bit 3, UE is bit 13 */
    USART2->CR1 = (1U << 3U) | (1U << 13U);
}

void Uart_SendString(const char* str) {
    if (str == NULL_PTR) return;
    
    while (*str) {
        /* Wait until TXE (Transmit data register empty) is set (Bit 7) */
        while (!GET_BIT(USART2->SR, 7U)); 
        USART2->DR = (uint32)(*str);
        str++;
    }
    /* Wait until TC (Transmission complete) is set (Bit 6) */
    while (!GET_BIT(USART2->SR, 6U));
}
