#ifndef MCU_GPIO_REG_H
#define MCU_GPIO_REG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx.h"

/* GPIO port base addresses */
#define MCU_GPIO_PORT_A_BASE    GPIOA
#define MCU_GPIO_PORT_B_BASE    GPIOB
#define MCU_GPIO_PORT_C_BASE    GPIOC
#define MCU_GPIO_PORT_D_BASE    GPIOD
#define MCU_GPIO_PORT_E_BASE    GPIOE
#define MCU_GPIO_PORT_F_BASE    GPIOF
#define MCU_GPIO_PORT_G_BASE    GPIOG
#define MCU_GPIO_PORT_H_BASE    GPIOH
#define MCU_GPIO_PORT_I_BASE    GPIOI

/* RCC GPIO port clock enable bits */
#define MCU_RCC_GPIOA_EN_BIT    RCC_AHB1ENR_GPIOAEN
#define MCU_RCC_GPIOB_EN_BIT    RCC_AHB1ENR_GPIOBEN
#define MCU_RCC_GPIOC_EN_BIT    RCC_AHB1ENR_GPIOCEN
#define MCU_RCC_GPIOD_EN_BIT    RCC_AHB1ENR_GPIODEN
#define MCU_RCC_GPIOE_EN_BIT    RCC_AHB1ENR_GPIOEEN
#define MCU_RCC_GPIOF_EN_BIT    RCC_AHB1ENR_GPIOFEN
#define MCU_RCC_GPIOG_EN_BIT    RCC_AHB1ENR_GPIOGEN
#define MCU_RCC_GPIOH_EN_BIT    RCC_AHB1ENR_GPIOHEN
#define MCU_RCC_GPIOI_EN_BIT    RCC_AHB1ENR_GPIOIEN

/* GPIO register bit offsets and masks */
#define MCU_GPIO_MODER_OFFSET(pin)          ((pin) * 2)
#define MCU_GPIO_OSPEEDR_OFFSET(pin)        ((pin) * 2)
#define MCU_GPIO_PUPDR_OFFSET(pin)          ((pin) * 2)

#define MCU_GPIO_MODER_MASK(pin)            (3UL << MCU_GPIO_MODER_OFFSET(pin))
#define MCU_GPIO_OSPEEDR_MASK(pin)          (3UL << MCU_GPIO_OSPEEDR_OFFSET(pin))
#define MCU_GPIO_PUPDR_MASK(pin)            (3UL << MCU_GPIO_PUPDR_OFFSET(pin))
#define MCU_GPIO_OTYPER_MASK(pin)           (1UL << (pin))
#define MCU_GPIO_IDR_MASK(pin)              (1UL << (pin))
#define MCU_GPIO_BSRR_SET_BIT(pin)          (1UL << (pin))
#define MCU_GPIO_BSRR_RESET_BIT(pin)        (1UL << ((pin) + 16))

/* GPIO port access helper macros */
#define MCU_GPIO_GET_PORT_BASE(port) \
    (((port) == 0) ? MCU_GPIO_PORT_A_BASE : \
     ((port) == 1) ? MCU_GPIO_PORT_B_BASE : \
     ((port) == 2) ? MCU_GPIO_PORT_C_BASE : \
     ((port) == 3) ? MCU_GPIO_PORT_D_BASE : \
     ((port) == 4) ? MCU_GPIO_PORT_E_BASE : \
     ((port) == 5) ? MCU_GPIO_PORT_F_BASE : \
     ((port) == 6) ? MCU_GPIO_PORT_G_BASE : \
     ((port) == 7) ? MCU_GPIO_PORT_H_BASE : \
     ((port) == 8) ? MCU_GPIO_PORT_I_BASE : \
     NULL)

#define MCU_GPIO_GET_RCC_EN_BIT(port) \
    (((port) == 0) ? MCU_RCC_GPIOA_EN_BIT : \
     ((port) == 1) ? MCU_RCC_GPIOB_EN_BIT : \
     ((port) == 2) ? MCU_RCC_GPIOC_EN_BIT : \
     ((port) == 3) ? MCU_RCC_GPIOD_EN_BIT : \
     ((port) == 4) ? MCU_RCC_GPIOE_EN_BIT : \
     ((port) == 5) ? MCU_RCC_GPIOF_EN_BIT : \
     ((port) == 6) ? MCU_RCC_GPIOG_EN_BIT : \
     ((port) == 7) ? MCU_RCC_GPIOH_EN_BIT : \
     ((port) == 8) ? MCU_RCC_GPIOI_EN_BIT : \
     0)

/* Register access validation */
#define MCU_GPIO_IS_VALID_PORT(port) \
    ((port) <= 8)

#define MCU_GPIO_IS_VALID_PIN(pin) \
    ((pin) <= 15)

#ifdef __cplusplus
}
#endif

#endif /* MCU_GPIO_REG_H */