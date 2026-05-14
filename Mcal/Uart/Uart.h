#ifndef UART_H_
#define UART_H_

#include "Std_Types.h"

void Uart_Init_USART2_115200(uint32 pclk_freq);
void Uart_SendString(const char* str);
void Uart_SendString_DMA(const char* str, uint16 len);

#endif /* UART_H_ */
