/**
 * @file    Gpio.c
 * @brief   MCAL — GPIO driver implementation (bare-metal CMSIS).
 */

#include "Gpio.h"
#include "Bit_Operations.h"

/* ------------------------------------------------------------------ */
/*  Clock enable                                                       */
/* ------------------------------------------------------------------ */
void Gpio_EnableClock(GPIO_TypeDef *port)
{
    if      (port == GPIOA) { SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN_Pos); }
    else if (port == GPIOB) { SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN_Pos); }
    else if (port == GPIOC) { SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOCEN_Pos); }
    else if (port == GPIOD) { SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIODEN_Pos); }
    else if (port == GPIOE) { SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOEEN_Pos); }
    else if (port == GPIOH) { SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOHEN_Pos); }
    /* else: unsupported port — silently ignored */
}

/* ------------------------------------------------------------------ */
/*  Pin configuration                                                  */
/* ------------------------------------------------------------------ */
void Gpio_ConfigPin(const Gpio_CfgType *cfg)
{
    GPIO_TypeDef *port = cfg->Port;
    uint8       pin  = cfg->Pin;

    /* MODER: 2 bits per pin */
    port->MODER &= ~(0x03U << (pin * 2U));
    port->MODER |=  ((uint32)cfg->Mode << (pin * 2U));

    /* OTYPER: 1 bit per pin */
    port->OTYPER &= ~(0x01U << pin);
    port->OTYPER |=  ((uint32)cfg->OType << pin);

    /* OSPEEDR: 2 bits per pin */
    port->OSPEEDR &= ~(0x03U << (pin * 2U));
    port->OSPEEDR |=  ((uint32)cfg->Speed << (pin * 2U));

    /* PUPDR: 2 bits per pin */
    port->PUPDR &= ~(0x03U << (pin * 2U));
    port->PUPDR |=  ((uint32)cfg->PuPd << (pin * 2U));

    /* Alternate-function register (AFR[0] for pins 0-7, AFR[1] for 8-15) */
    if (cfg->Mode == GPIO_MODE_AF) {
        uint8 idx = (pin < 8U) ? 0U : 1U;
        uint8 pos = (pin % 8U) * 4U;

        port->AFR[idx] &= ~(0x0FU << pos);
        port->AFR[idx] |=  ((uint32)cfg->AltFunc << pos);
    }
}

/* ------------------------------------------------------------------ */
/*  Digital I/O                                                        */
/* ------------------------------------------------------------------ */
uint8 Gpio_ReadPin(GPIO_TypeDef *port, uint8 pin)
{
    return GET_BIT(port->IDR, pin);
}

void Gpio_WriteHigh(GPIO_TypeDef *port, uint8 pin)
{
    port->BSRR = (1U << pin);              /* atomic set via BSRR */
}

void Gpio_WriteLow(GPIO_TypeDef *port, uint8 pin)
{
    port->BSRR = (1U << (pin + 16U));      /* atomic reset via BSRR[31:16] */
}

void Gpio_TogglePin(GPIO_TypeDef *port, uint8 pin)
{
    TOG_BIT(port->ODR, pin);
}
