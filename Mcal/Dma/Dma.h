/**
 * @file    Dma.h
 * @brief   MCAL — DMA1 Stream 6 driver for USART2 TX.
 *
 * On STM32F401, USART2_TX is mapped to DMA1 Stream 6, Channel 4.
 * This driver provides a fire-and-forget non-blocking TX path.
 */

#ifndef DMA_H_
#define DMA_H_

#include "Std_Types.h"

/**
 * @brief  Initialise DMA1 Stream 6 for USART2 TX transfers.
 *         Must be called once after Uart_Init_USART2_115200().
 */
void Dma_Uart2Tx_Init(void);

/**
 * @brief  Start a DMA transfer from memory to USART2->DR.
 * @param  buf  Pointer to the source buffer (must remain valid until TC).
 * @param  len  Number of bytes to transmit.
 *
 * If a previous transfer is still in progress, this call is a no-op.
 */
void Dma_Uart2Tx_Start(const uint8* buf, uint16 len);

/**
 * @brief  Returns TRUE if a DMA TX transfer is currently in progress.
 */
boolean Dma_Uart2Tx_IsBusy(void);

#endif /* DMA_H_ */
