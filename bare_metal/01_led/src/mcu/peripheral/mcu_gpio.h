#ifndef MCU_GPIO_H
#define MCU_GPIO_H

#include "stm32f4xx.h"
#include <stdint.h>

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
     0)

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

#define MCU_GPIO_MODER_MASK(pin)            (3UL << ((pin) * 2))
#define MCU_GPIO_OSPEEDR_MASK(pin)          (3UL << ((pin) * 2))
#define MCU_GPIO_PUPDR_MASK(pin)            (3UL << ((pin) * 2))
#define MCU_GPIO_OTYPER_MASK(pin)           (1UL << (pin))
#define MCU_GPIO_IDR_MASK(pin)              (1UL << (pin))
#define MCU_GPIO_BSRR_SET_BIT(pin)          (1UL << (pin))
#define MCU_GPIO_BSRR_RESET_BIT(pin)        (1UL << ((pin) + 16))

#define MCU_GPIO_AFR_INDEX(pin)             ((pin) >> 3)      // 0 for pins 0-7, 1 for pins 8-15
#define MCU_GPIO_AFR_OFFSET(pin)            (((pin) & 0x07) * 4)  // 0, 4, 8, 12, 16, 20, 24, 28
#define MCU_GPIO_AFR_MASK(pin)              (0x0F << MCU_GPIO_AFR_OFFSET(pin))

typedef enum {
    MCU_GPIO_PORT_A = 0,
    MCU_GPIO_PORT_B = 1,
    MCU_GPIO_PORT_C = 2,
    MCU_GPIO_PORT_D = 3,
    MCU_GPIO_PORT_E = 4,
    MCU_GPIO_PORT_F = 5,
    MCU_GPIO_PORT_G = 6,
    MCU_GPIO_PORT_H = 7,
    MCU_GPIO_PORT_I = 8
} mcu_gpio_port_t;

typedef enum {
    MCU_GPIO_PIN_0 = 0,
    MCU_GPIO_PIN_1 = 1,
    MCU_GPIO_PIN_2 = 2,
    MCU_GPIO_PIN_3 = 3,
    MCU_GPIO_PIN_4 = 4,
    MCU_GPIO_PIN_5 = 5,
    MCU_GPIO_PIN_6 = 6,
    MCU_GPIO_PIN_7 = 7,
    MCU_GPIO_PIN_8 = 8,
    MCU_GPIO_PIN_9 = 9,
    MCU_GPIO_PIN_10 = 10,
    MCU_GPIO_PIN_11 = 11,
    MCU_GPIO_PIN_12 = 12,
    MCU_GPIO_PIN_13 = 13,
    MCU_GPIO_PIN_14 = 14,
    MCU_GPIO_PIN_15 = 15
} mcu_gpio_pin_t;

typedef enum {
    MCU_GPIO_AF0 = 0,
    MCU_GPIO_AF1 = 1,
    MCU_GPIO_AF2 = 2,
    MCU_GPIO_AF3 = 3,
    MCU_GPIO_AF4 = 4,
    MCU_GPIO_AF5 = 5,
    MCU_GPIO_AF6 = 6,
    MCU_GPIO_AF7 = 7,
    MCU_GPIO_AF8 = 8,
    MCU_GPIO_AF9 = 9,
    MCU_GPIO_AF10 = 10,
    MCU_GPIO_AF11 = 11,
    MCU_GPIO_AF12 = 12,
    MCU_GPIO_AF13 = 13,
    MCU_GPIO_AF14 = 14,
    MCU_GPIO_AF15 = 15
} mcu_gpio_af_t;

typedef enum {
    MCU_GPIO_MODE_INPUT = 0,
    MCU_GPIO_MODE_OUTPUT = 1,
    MCU_GPIO_MODE_ALT = 2,
    MCU_GPIO_MODE_ANALOG = 3
} mcu_gpio_mode_t;

typedef enum {
    MCU_GPIO_OTYPE_PUSHPULL = 0,
    MCU_GPIO_OTYPE_OPENDRAIN = 1
} mcu_gpio_otype_t;

typedef enum {
    MCU_GPIO_OSPEED_LOW = 0,
    MCU_GPIO_OSPEED_MEDIUM = 1,
    MCU_GPIO_OSPEED_FAST = 2,
    MCU_GPIO_OSPEED_HIGH = 3
} mcu_gpio_ospeed_t;

typedef enum {
    MCU_GPIO_PUPD_NONE = 0,
    MCU_GPIO_PUPD_UP = 1,
    MCU_GPIO_PUPD_DOWN = 2,
    MCU_GPIO_PUPD_RES = 3
} mcu_gpio_pupd_t;

typedef enum {
    MCU_GPIO_PIN_RESET = 0,
    MCU_GPIO_PIN_SET = 1
} mcu_gpio_state_t;

typedef struct
{
    mcu_gpio_mode_t mode;
    mcu_gpio_otype_t otype;
    mcu_gpio_ospeed_t ospeed;
    mcu_gpio_pupd_t pupd;
    mcu_gpio_af_t af;
} mcu_gpio_config_t;

void mcu_gpio_config(mcu_gpio_port_t port, mcu_gpio_pin_t pin, const mcu_gpio_config_t* p_config);
void mcu_gpio_set(mcu_gpio_port_t port, mcu_gpio_pin_t pin, mcu_gpio_state_t state);
void mcu_gpio_toggle(mcu_gpio_port_t port, mcu_gpio_pin_t pin);

#endif