#ifndef UART_H_
#define UART_H_

#include "Std_Types.h"

void Uart_Init_USART2_115200(uint32 pclk_freq);
void Uart_SendString(const char* str);

#endif /* UART_H_ */
