#ifndef UART_H_
#define UART_H_

#include "Std_Types.h"

/* Initializes USART2 at the given baud rate (115200), PA2 for TX */
void Uart2_Init(uint32 baudRate);

/* Sends a null-terminated string via USART2 (Blocking initially) */
void Uart2_SendString(const char* str);

#endif /* UART_H_ */
