#include "Uart.h"
#include "Mcu_Hw.h"
#include "Bit_Operations.h"
#include "Gpio.h"

void Uart_Init_USART2_115200(uint32 pclk_freq) {
    /* 1. Enable USART2 Clock */
    SET_BIT(RCC->APB1ENR, RCC_APB1ENR_USART2EN_Pos); 
    
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
    
    /* 4. Enable USART and Transmitter */
    USART2->CR1 = USART_CR1_TE | USART_CR1_UE;
}

void Uart_SendString(const char* str) {
    if (str == NULL_PTR) return;
    
    while (*str) {
        /* Wait until TXE (Transmit data register empty) is set */
        while (!GET_BIT(USART2->SR, USART_SR_TXE_Pos)); 
        USART2->DR = (uint32)(*str);
        str++;
    }
    /* Wait until TC (Transmission complete) is set */
    while (!GET_BIT(USART2->SR, USART_SR_TC_Pos));
}
