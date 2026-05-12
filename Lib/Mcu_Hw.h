/**
 * @file    Mcu_Hw.h
 * @brief   Hardware definitions for STM32F401xx.
 *          Replaces ST's CMSIS device header (stm32f4xx.h).
 */

#ifndef MCU_HW_H_
#define MCU_HW_H_

#include "Std_Types.h"

/* ------------------------------------------------------------------------- */
/* Core & Peripheral Base Addresses                                          */
/* ------------------------------------------------------------------------- */
#define PERIPH_BASE           0x40000000U
#define APB1PERIPH_BASE       PERIPH_BASE
#define APB2PERIPH_BASE       (PERIPH_BASE + 0x00010000U)
#define AHB1PERIPH_BASE       (PERIPH_BASE + 0x00020000U)

/* APB1 Peripherals */
#define TIM2_BASE             (APB1PERIPH_BASE + 0x0000U)
#define TIM3_BASE             (APB1PERIPH_BASE + 0x0400U)
#define TIM4_BASE             (APB1PERIPH_BASE + 0x0800U)
#define TIM5_BASE             (APB1PERIPH_BASE + 0x0C00U)
#define USART2_BASE           (APB1PERIPH_BASE + 0x4400U)

/* APB2 Peripherals */
#define SYSCFG_BASE           (APB2PERIPH_BASE + 0x3800U)
#define EXTI_BASE             (APB2PERIPH_BASE + 0x3C00U)
#define SPI1_BASE             (APB2PERIPH_BASE + 0x3000U)

/* AHB1 Peripherals */
#define GPIOA_BASE            (AHB1PERIPH_BASE + 0x0000U)
#define GPIOB_BASE            (AHB1PERIPH_BASE + 0x0400U)
#define GPIOC_BASE            (AHB1PERIPH_BASE + 0x0800U)
#define GPIOD_BASE            (AHB1PERIPH_BASE + 0x0C00U)
#define GPIOE_BASE            (AHB1PERIPH_BASE + 0x1000U)
#define GPIOH_BASE            (AHB1PERIPH_BASE + 0x1C00U)
#define RCC_BASE              (AHB1PERIPH_BASE + 0x3800U)

/* Core Peripherals */
#define SCS_BASE              0xE000E000U
#define NVIC_BASE             (SCS_BASE + 0x0100U)
#define SCB_BASE              (SCS_BASE + 0x0D00U)
#define SysTick_BASE          (SCS_BASE + 0x0010U)

/* ------------------------------------------------------------------------- */
/* Register Struct Definitions                                               */
/* ------------------------------------------------------------------------- */

/* GPIO */
typedef struct {
    volatile uint32_t MODER;
    volatile uint32_t OTYPER;
    volatile uint32_t OSPEEDR;
    volatile uint32_t PUPDR;
    volatile uint32_t IDR;
    volatile uint32_t ODR;
    volatile uint32_t BSRR;
    volatile uint32_t LCKR;
    volatile uint32_t AFR[2];
} GPIO_TypeDef;

/* RCC */
typedef struct {
    volatile uint32_t CR;
    volatile uint32_t PLLCFGR;
    volatile uint32_t CFGR;
    volatile uint32_t CIR;
    volatile uint32_t AHB1RSTR;
    volatile uint32_t AHB2RSTR;
    volatile uint32_t Reserved0;
    volatile uint32_t Reserved1;
    volatile uint32_t APB1RSTR;
    volatile uint32_t APB2RSTR;
    volatile uint32_t Reserved2[2];
    volatile uint32_t AHB1ENR;
    volatile uint32_t AHB2ENR;
    volatile uint32_t Reserved3[2];
    volatile uint32_t APB1ENR;
    volatile uint32_t APB2ENR;
    volatile uint32_t Reserved4[2];
    volatile uint32_t AHB1LPENR;
    volatile uint32_t AHB2LPENR;
    volatile uint32_t Reserved5[2];
    volatile uint32_t APB1LPENR;
    volatile uint32_t APB2LPENR;
    volatile uint32_t Reserved6[2];
    volatile uint32_t BDCR;
    volatile uint32_t CSR;
    volatile uint32_t Reserved7[2];
    volatile uint32_t SSCGR;
    volatile uint32_t PLLI2SCFGR;
} RCC_TypeDef;

/* EXTI */
typedef struct {
    volatile uint32_t IMR;
    volatile uint32_t EMR;
    volatile uint32_t RTSR;
    volatile uint32_t FTSR;
    volatile uint32_t SWIER;
    volatile uint32_t PR;
} EXTI_TypeDef;

/* SYSCFG */
typedef struct {
    volatile uint32_t MEMRMP;
    volatile uint32_t PMC;
    volatile uint32_t EXTICR[4];
    volatile uint32_t CMPCR;
} SYSCFG_TypeDef;

/* NVIC */
typedef struct {
    volatile uint32_t ISER[8];
    volatile uint32_t Reserved0[24];
    volatile uint32_t ICER[8];
    volatile uint32_t Reserved1[24];
    volatile uint32_t ISPR[8];
    volatile uint32_t Reserved2[24];
    volatile uint32_t ICPR[8];
    volatile uint32_t Reserved3[24];
    volatile uint32_t IABR[8];
    volatile uint32_t Reserved4[56];
    volatile uint8_t  IP[240];
    volatile uint32_t Reserved5[644];
    volatile uint32_t STIR;
} NVIC_Type;

/* SCB */
typedef struct {
    volatile uint32_t CPUID;
    volatile uint32_t ICSR;
    volatile uint32_t VTOR;
    volatile uint32_t AIRCR;
    volatile uint32_t SCR;
    volatile uint32_t CCR;
    volatile uint8_t  SHP[12];
    volatile uint32_t SHCSR;
    volatile uint32_t CFSR;
    volatile uint32_t HFSR;
    volatile uint32_t DFSR;
    volatile uint32_t MMFAR;
    volatile uint32_t BFAR;
    volatile uint32_t AFSR;
} SCB_Type;

/* SysTick */
typedef struct {
    volatile uint32_t CTRL;
    volatile uint32_t LOAD;
    volatile uint32_t VAL;
    volatile uint32_t CALIB;
} SysTick_Type;

/* TIM */
typedef struct {
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t SMCR;
    volatile uint32_t DIER;
    volatile uint32_t SR;
    volatile uint32_t EGR;
    volatile uint32_t CCMR1;
    volatile uint32_t CCMR2;
    volatile uint32_t CCER;
    volatile uint32_t CNT;
    volatile uint32_t PSC;
    volatile uint32_t ARR;
    volatile uint32_t Reserved0;
    volatile uint32_t CCR1;
    volatile uint32_t CCR2;
    volatile uint32_t CCR3;
    volatile uint32_t CCR4;
    volatile uint32_t Reserved1;
    volatile uint32_t DCR;
    volatile uint32_t DMAR;
    volatile uint32_t OR;
} TIM_TypeDef;

/* SPI */
typedef struct {
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t SR;
    volatile uint32_t DR;
    volatile uint32_t CRCPR;
    volatile uint32_t RXCRCR;
    volatile uint32_t TXCRCR;
    volatile uint32_t I2SCFGR;
    volatile uint32_t I2SPR;
} SPI_TypeDef;

/* USART */
typedef struct {
    volatile uint32_t SR;
    volatile uint32_t DR;
    volatile uint32_t BRR;
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t CR3;
    volatile uint32_t GTPR;
} USART_TypeDef;

/* ------------------------------------------------------------------------- */
/* Peripheral Pointers                                                       */
/* ------------------------------------------------------------------------- */
#define GPIOA               ((GPIO_TypeDef *) GPIOA_BASE)
#define GPIOB               ((GPIO_TypeDef *) GPIOB_BASE)
#define GPIOC               ((GPIO_TypeDef *) GPIOC_BASE)
#define GPIOD               ((GPIO_TypeDef *) GPIOD_BASE)
#define GPIOE               ((GPIO_TypeDef *) GPIOE_BASE)
#define GPIOH               ((GPIO_TypeDef *) GPIOH_BASE)

#define RCC                 ((RCC_TypeDef *) RCC_BASE)
#define EXTI                ((EXTI_TypeDef *) EXTI_BASE)
#define SYSCFG              ((SYSCFG_TypeDef *) SYSCFG_BASE)
#define NVIC                ((NVIC_Type *) NVIC_BASE)
#define SCB                 ((SCB_Type *) SCB_BASE)
#define SysTick             ((SysTick_Type *) SysTick_BASE)

#define TIM2                ((TIM_TypeDef *) TIM2_BASE)
#define TIM3                ((TIM_TypeDef *) TIM3_BASE)
#define TIM4                ((TIM_TypeDef *) TIM4_BASE)
#define TIM5                ((TIM_TypeDef *) TIM5_BASE)

#define SPI1                ((SPI_TypeDef *) SPI1_BASE)
#define USART2              ((USART_TypeDef *) USART2_BASE)

/* ------------------------------------------------------------------------- */
/* Interrupt Numbers (IRQn_Type)                                             */
/* ------------------------------------------------------------------------- */
typedef enum {
    NonMaskableInt_IRQn   = -14,
    MemoryManagement_IRQn = -12,
    BusFault_IRQn         = -11,
    UsageFault_IRQn       = -10,
    SVCall_IRQn           = -5,
    DebugMonitor_IRQn     = -4,
    PendSV_IRQn           = -2,
    SysTick_IRQn          = -1,
    WWDG_IRQn             = 0,
    PVD_IRQn              = 1,
    TAMP_STAMP_IRQn       = 2,
    RTC_WKUP_IRQn         = 3,
    FLASH_IRQn            = 4,
    RCC_IRQn              = 5,
    EXTI0_IRQn            = 6,
    EXTI1_IRQn            = 7,
    EXTI2_IRQn            = 8,
    EXTI3_IRQn            = 9,
    EXTI4_IRQn            = 10,
    DMA1_Stream0_IRQn     = 11,
    DMA1_Stream1_IRQn     = 12,
    DMA1_Stream2_IRQn     = 13,
    DMA1_Stream3_IRQn     = 14,
    DMA1_Stream4_IRQn     = 15,
    DMA1_Stream5_IRQn     = 16,
    DMA1_Stream6_IRQn     = 17,
    ADC_IRQn              = 18,
    EXTI9_5_IRQn          = 23,
    TIM1_BRK_TIM9_IRQn    = 24,
    TIM1_UP_TIM10_IRQn    = 25,
    TIM1_TRG_COM_TIM11_IRQn= 26,
    TIM1_CC_IRQn          = 27,
    TIM2_IRQn             = 28,
    TIM3_IRQn             = 29,
    TIM4_IRQn             = 30,
    I2C1_EV_IRQn          = 31,
    I2C1_ER_IRQn          = 32,
    I2C2_EV_IRQn          = 33,
    I2C2_ER_IRQn          = 34,
    SPI1_IRQn             = 35,
    SPI2_IRQn             = 36,
    USART1_IRQn           = 37,
    USART2_IRQn           = 38,
    EXTI15_10_IRQn        = 40,
    RTC_Alarm_IRQn        = 41,
    OTG_FS_WKUP_IRQn      = 42,
    DMA1_Stream7_IRQn     = 47,
    SDIO_IRQn             = 49,
    TIM5_IRQn             = 50,
    SPI3_IRQn             = 51,
    DMA2_Stream0_IRQn     = 56,
    DMA2_Stream1_IRQn     = 57,
    DMA2_Stream2_IRQn     = 58,
    DMA2_Stream3_IRQn     = 59,
    DMA2_Stream4_IRQn     = 60,
    OTG_FS_IRQn           = 67,
    DMA2_Stream5_IRQn     = 68,
    DMA2_Stream6_IRQn     = 69,
    DMA2_Stream7_IRQn     = 70,
    USART6_IRQn           = 71,
    I2C3_EV_IRQn          = 72,
    I2C3_ER_IRQn          = 73,
    FPU_IRQn              = 81,
    SPI4_IRQn             = 84
} IRQn_Type;

/* ------------------------------------------------------------------------- */
/* Bit Positions & Masks                                                     */
/* ------------------------------------------------------------------------- */

/* RCC AHB1ENR */
#define RCC_AHB1ENR_GPIOAEN_Pos   0U
#define RCC_AHB1ENR_GPIOBEN_Pos   1U
#define RCC_AHB1ENR_GPIOCEN_Pos   2U
#define RCC_AHB1ENR_GPIODEN_Pos   3U
#define RCC_AHB1ENR_GPIOEEN_Pos   4U
#define RCC_AHB1ENR_GPIOHEN_Pos   7U

/* RCC APB2ENR */
#define RCC_APB2ENR_SYSCFGEN_Pos  14U

/* RCC APB1ENR */
#define RCC_APB1ENR_TIM2EN_Pos    0U
#define RCC_APB1ENR_TIM3EN_Pos    1U
#define RCC_APB1ENR_TIM4EN_Pos    2U
#define RCC_APB1ENR_TIM5EN_Pos    3U

/* TIM CR1 */
#define TIM_CR1_CEN_Pos           0U
#define TIM_CR1_ARPE_Pos          7U

/* TIM EGR */
#define TIM_EGR_UG_Pos            0U

/* TIM CCER */
#define TIM_CCER_CC1E_Pos         0U
#define TIM_CCER_CC2E_Pos         4U
#define TIM_CCER_CC3E_Pos         8U
#define TIM_CCER_CC4E_Pos         12U

/* TIM CCMR1 */
#define TIM_CCMR1_OC1PE_Pos       3U
#define TIM_CCMR1_OC1PE_Msk       (1UL << TIM_CCMR1_OC1PE_Pos)
#define TIM_CCMR1_OC1M_Pos        4U
#define TIM_CCMR1_OC1M_Msk        (7UL << TIM_CCMR1_OC1M_Pos)

#define TIM_CCMR1_OC2PE_Pos       11U
#define TIM_CCMR1_OC2PE_Msk       (1UL << TIM_CCMR1_OC2PE_Pos)
#define TIM_CCMR1_OC2M_Pos        12U
#define TIM_CCMR1_OC2M_Msk        (7UL << TIM_CCMR1_OC2M_Pos)

/* TIM CCMR2 */
#define TIM_CCMR2_OC3PE_Pos       3U
#define TIM_CCMR2_OC3PE_Msk       (1UL << TIM_CCMR2_OC3PE_Pos)
#define TIM_CCMR2_OC3M_Pos        4U
#define TIM_CCMR2_OC3M_Msk        (7UL << TIM_CCMR2_OC3M_Pos)

#define TIM_CCMR2_OC4PE_Pos       11U
#define TIM_CCMR2_OC4PE_Msk       (1UL << TIM_CCMR2_OC4PE_Pos)
#define TIM_CCMR2_OC4M_Pos        12U
#define TIM_CCMR2_OC4M_Msk        (7UL << TIM_CCMR2_OC4M_Pos)

#endif /* MCU_HW_H_ */