/**
 * @file    Dma.c
 * @brief   MCAL — DMA1 Stream 6 / Channel 4 for USART2 TX.
 *
 * ┌──────────────────────────────────────────────────────────┐
 * │  DMA1 Stream 6, Channel 4 = USART2_TX (STM32F401)      │
 * │                                                          │
 * │  Direction:  Memory → Peripheral                         │
 * │  Data size:  8-bit (byte)                                │
 * │  Memory:     Increment                                   │
 * │  Peripheral: Fixed (USART2->DR)                          │
 * │  Mode:       Normal (single-shot, not circular)          │
 * │  Interrupt:  Transfer Complete (TCIE)                     │
 * └──────────────────────────────────────────────────────────┘
 */

#include "Dma.h"
#include "Mcu_Hw.h"
#include "Bit_Operations.h"
#include "Nvic.h"

/* ---- DMA1 Stream 6 CR bit positions (from RM0368) ---- */
#define DMA_SxCR_EN_Pos       0U
#define DMA_SxCR_TCIE_Pos     4U    /* Transfer Complete IE */
#define DMA_SxCR_DIR_Pos      6U    /* 01 = Memory-to-Peripheral */
#define DMA_SxCR_MINC_Pos     10U   /* Memory increment mode */
#define DMA_SxCR_CHSEL_Pos    25U   /* Channel selection */

/* ---- DMA1 HISR / HIFCR bit positions for Stream 6 ---- */
#define DMA_HISR_TCIF6_Pos    21U
#define DMA_HIFCR_CTCIF6_Pos  21U
#define DMA_HIFCR_CHTIF6_Pos  20U
#define DMA_HIFCR_CTEIF6_Pos  19U
#define DMA_HIFCR_CDMEIF6_Pos 18U
#define DMA_HIFCR_CFEIF6_Pos  16U

/* ---- Module-level busy flag ---- */
static volatile boolean g_DmaTxBusy = FALSE;

/* =================================================================== */
/*  Initialisation                                                      */
/* =================================================================== */
void Dma_Uart2Tx_Init(void) {
    /* Enable DMA1 clock */
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_DMA1EN_Pos);

    /* Ensure stream is disabled before configuration */
    CLR_BIT(DMA1_Stream6->CR, DMA_SxCR_EN_Pos);

    /* Wait until EN reads 0 (hardware clears it) */
    while (DMA1_Stream6->CR & (1U << DMA_SxCR_EN_Pos)) { /* spin */ }

    /* Configure Stream 6:
     *  - Channel 4         (bits [27:25] = 4)
     *  - Memory-to-Periph  (bits [7:6]   = 01)
     *  - Memory increment  (bit 10       = 1)
     *  - 8-bit data sizes  (MSIZE/PSIZE  = 00, default)
     *  - Transfer Complete interrupt enabled
     */
    DMA1_Stream6->CR = (4U << DMA_SxCR_CHSEL_Pos)  /* Channel 4 */
                     | (1U << DMA_SxCR_DIR_Pos)     /* Mem→Periph */
                     | (1U << DMA_SxCR_MINC_Pos)    /* Memory increment */
                     | (1U << DMA_SxCR_TCIE_Pos);   /* TC interrupt */

    /* Peripheral address = USART2->DR (fixed, not incrementing) */
    DMA1_Stream6->PAR = (uint32)&(USART2->DR);

    /* Enable DMA1_Stream6 IRQ in NVIC (low priority — telemetry is non-critical) */
    Nvic_SetPriority(DMA1_Stream6_IRQn, 7U, 0U);
    Nvic_EnableIrq(DMA1_Stream6_IRQn);

    /* Enable USART2 DMA TX request */
    SET_BIT(USART2->CR3, USART_CR3_DMAT_Pos);

    g_DmaTxBusy = FALSE;
}

/* =================================================================== */
/*  Start a transfer                                                    */
/* =================================================================== */
void Dma_Uart2Tx_Start(const uint8* buf, uint16 len) {
    if (buf == NULL_PTR || len == 0U) { return; }
    if (g_DmaTxBusy) { return; }  /* previous transfer still in flight */

    g_DmaTxBusy = TRUE;

    /* Disable stream before reconfiguring */
    CLR_BIT(DMA1_Stream6->CR, DMA_SxCR_EN_Pos);
    while (DMA1_Stream6->CR & (1U << DMA_SxCR_EN_Pos)) { /* spin */ }

    /* Clear all Stream 6 interrupt flags */
    DMA1->HIFCR = (1U << DMA_HIFCR_CTCIF6_Pos)
                | (1U << DMA_HIFCR_CHTIF6_Pos)
                | (1U << DMA_HIFCR_CTEIF6_Pos)
                | (1U << DMA_HIFCR_CDMEIF6_Pos)
                | (1U << DMA_HIFCR_CFEIF6_Pos);

    /* Set source address and transfer count */
    DMA1_Stream6->M0AR = (uint32)buf;
    DMA1_Stream6->NDTR = (uint32)len;

    /* Enable the stream → transfer begins immediately */
    SET_BIT(DMA1_Stream6->CR, DMA_SxCR_EN_Pos);
}

/* =================================================================== */
/*  Busy query                                                          */
/* =================================================================== */
boolean Dma_Uart2Tx_IsBusy(void) {
    return g_DmaTxBusy;
}

/* =================================================================== */
/*  Transfer Complete ISR                                               */
/* =================================================================== */
void DMA1_Stream6_IRQHandler(void) {
    if (DMA1->HISR & (1U << DMA_HISR_TCIF6_Pos)) {
        /* Clear TC flag */
        DMA1->HIFCR = (1U << DMA_HIFCR_CTCIF6_Pos);

        /* Disable stream */
        CLR_BIT(DMA1_Stream6->CR, DMA_SxCR_EN_Pos);

        g_DmaTxBusy = FALSE;
    }
}
