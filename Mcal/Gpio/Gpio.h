/**
 * @file    Gpio.h
 * @brief   MCAL — Thin GPIO driver for STM32F401xE (bare-metal CMSIS).
 *
 * Provides pin-level configuration and I/O without ST HAL dependency.
 */

#ifndef GPIO_H_
#define GPIO_H_

#include "Std_Types.h"
#include "stm32f4xx.h"      /* CMSIS device header */

/* ------------------------------------------------------------------ */
/*  Pin Mode                                                           */
/* ------------------------------------------------------------------ */
typedef enum {
    GPIO_MODE_INPUT  = 0x00U,
    GPIO_MODE_OUTPUT = 0x01U,
    GPIO_MODE_AF     = 0x02U,
    GPIO_MODE_ANALOG = 0x03U
} Gpio_ModeType;

/* ------------------------------------------------------------------ */
/*  Output Type                                                        */
/* ------------------------------------------------------------------ */
typedef enum {
    GPIO_OTYPE_PUSHPULL  = 0x00U,
    GPIO_OTYPE_OPENDRAIN = 0x01U
} Gpio_OTypeType;

/* ------------------------------------------------------------------ */
/*  Pull-Up / Pull-Down                                                */
/* ------------------------------------------------------------------ */
typedef enum {
    GPIO_PUPD_NONE = 0x00U,
    GPIO_PUPD_PU   = 0x01U,
    GPIO_PUPD_PD   = 0x02U
} Gpio_PuPdType;

/* ------------------------------------------------------------------ */
/*  Speed                                                              */
/* ------------------------------------------------------------------ */
typedef enum {
    GPIO_SPEED_LOW  = 0x00U,
    GPIO_SPEED_MED  = 0x01U,
    GPIO_SPEED_HIGH = 0x03U
} Gpio_SpeedType;

/* ------------------------------------------------------------------ */
/*  Pin configuration aggregate                                        */
/* ------------------------------------------------------------------ */
typedef struct {
    GPIO_TypeDef   *Port;
    uint8_t         Pin;        /* 0 – 15 */
    Gpio_ModeType   Mode;
    Gpio_OTypeType  OType;
    Gpio_PuPdType   PuPd;
    Gpio_SpeedType  Speed;
    uint8_t         AltFunc;    /* AF0 – AF15, only used when Mode == AF */
} Gpio_CfgType;

/* ------------------------------------------------------------------ */
/*  API                                                                */
/* ------------------------------------------------------------------ */

/**
 * @brief Enable the AHB1 clock for the given GPIO port.
 */
void Gpio_EnableClock(GPIO_TypeDef *port);

/**
 * @brief Configure a single GPIO pin per the supplied descriptor.
 */
void Gpio_ConfigPin(const Gpio_CfgType *cfg);

/**
 * @brief Read the IDR bit for the given pin.
 * @return 0 or 1
 */
uint8_t Gpio_ReadPin(GPIO_TypeDef *port, uint8_t pin);

/**
 * @brief Set (1) the ODR bit for the given pin.
 */
void Gpio_WriteHigh(GPIO_TypeDef *port, uint8_t pin);

/**
 * @brief Clear (0) the ODR bit for the given pin.
 */
void Gpio_WriteLow(GPIO_TypeDef *port, uint8_t pin);

/**
 * @brief Toggle the ODR bit for the given pin.
 */
void Gpio_TogglePin(GPIO_TypeDef *port, uint8_t pin);

#endif /* GPIO_H_ */
