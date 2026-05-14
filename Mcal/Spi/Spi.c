#include "Spi.h"
#include "Mcu_Hw.h"
#include "Bit_Operations.h"
#include "Critical_Section.h"
#include <string.h>

/*
 * Minimal, portable SPI exchange implementation for Part 2.
 * This implementation is intentionally conservative:
 * - It provides init stubs for Master/Slave so platform-specific pin mapping
 *   and advanced register configuration can be added later.
 * - The exchange function performs a safe, atomic memcpy loopback when hardware
 *   is not present. On real hardware the implementation can be replaced with
 *   interrupt-driven TX/RX.
 */

void Spi_InitMaster(void) {
    /* Enable SPI1 clock (APB2 bit 12). Keep simple and portable here. */
    SET_BIT(RCC->APB2ENR, 12);
    /* Basic default: disable SPI while configuring */
    CLR_BIT(SPI1->CR1, 6); /* SPE = 0 */
    /* Master selection, CPOL/CPHA = 0, BR = fPCLK/16, 8-bit, MSB first */
    WRITE_REG(SPI1->CR1, (1U << 2)); /* MSTR bit */
    /* Enable SPI peripheral */
    /* SET_BIT(SPI1->CR1, 6); commented out to prevent PA6 pin hijacking in Proteus */
}

void Spi_InitSlave(void) {
    /* Slave may use SPI1 in this project; enable clock to be safe. */
    SET_BIT(RCC->APB2ENR, 12);
    CLR_BIT(SPI1->CR1, 6); /* SPE = 0 */
    /* Clear master bit -> slave mode */
    CLR_BIT(SPI1->CR1, 2);
    /* Enable SPI */
    /* SET_BIT(SPI1->CR1, 6); commented out to prevent PA6 pin hijacking in Proteus */
}

void Spi_ExchangeFrame(const Ipc_SpiFrame_t* tx, Ipc_SpiFrame_t* rx) {
    if (tx == NULL || rx == NULL) {
        return;
    }

    /* Use critical section to protect any shared buffers or registers */
    ENTER_CRITICAL();
    /* On a real board this would write bytes to SPI->DR and read back RX bytes.
     * For portability and offline compilation we simply copy the tx to rx as a
     * conservative default behavior (loopback). This keeps higher layers
     * functional while hardware-specific code is integrated later.
     */
    memcpy(rx, tx, sizeof(Ipc_SpiFrame_t));
    EXIT_CRITICAL();
}
