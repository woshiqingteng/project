#ifndef MCU_GPIO_REG_H
#define MCU_GPIO_REG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx.h"

/* GPIO port access helper macros */
#define MCU_GPIO_GET_PORT(port) \
    (((port) == 0) ? GPIOA : \
     ((port) == 1) ? GPIOB : \
     ((port) == 2) ? GPIOC : \
     ((port) == 3) ? GPIOD : \
     ((port) == 4) ? GPIOE : \
     ((port) == 5) ? GPIOF : \
     ((port) == 6) ? GPIOG : \
     ((port) == 7) ? GPIOH : \
     ((port) == 8) ? GPIOI : \
     NULL)

#define MCU_GPIO_GET_RCC_EN(port) \
    (((port) == 0) ? RCC_AHB1ENR_GPIOAEN : \
     ((port) == 1) ? RCC_AHB1ENR_GPIOBEN : \
     ((port) == 2) ? RCC_AHB1ENR_GPIOCEN : \
     ((port) == 3) ? RCC_AHB1ENR_GPIODEN : \
     ((port) == 4) ? RCC_AHB1ENR_GPIOEEN : \
     ((port) == 5) ? RCC_AHB1ENR_GPIOFEN : \
     ((port) == 6) ? RCC_AHB1ENR_GPIOGEN : \
     ((port) == 7) ? RCC_AHB1ENR_GPIOHEN : \
     ((port) == 8) ? RCC_AHB1ENR_GPIOIEN : \
     0)

/* GPIO register bit masks */
#define MCU_GPIO_MODER_MASK(pin)            (3UL << ((pin) * 2))
#define MCU_GPIO_OSPEEDR_MASK(pin)          (3UL << ((pin) * 2))
#define MCU_GPIO_PUPDR_MASK(pin)            (3UL << ((pin) * 2))
#define MCU_GPIO_OTYPER_MASK(pin)           (1UL << (pin))
#define MCU_GPIO_IDR_MASK(pin)              (1UL << (pin))
#define MCU_GPIO_BSRR_SET_BIT(pin)          (1UL << (pin))
#define MCU_GPIO_BSRR_RESET_BIT(pin)        (1UL << ((pin) + 16))

/* GPIO AFR register helpers */
#define MCU_GPIO_AFR_INDEX(pin)             ((pin) >> 3)      // 0 for pins 0-7, 1 for pins 8-15
#define MCU_GPIO_AFR_OFFSET(pin)            (((pin) & 0x07) * 4)  // 0, 4, 8, 12, 16, 20, 24, 28
#define MCU_GPIO_AFR_MASK(pin)              (0x0F << MCU_GPIO_AFR_OFFSET(pin))

#ifdef __cplusplus
}
#endif

#endif