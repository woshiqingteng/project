#ifndef BSP_GPIO_H
#define BSP_GPIO_H

#include "mcu_gpio.h"

#define BSP_LED0_PORT      MCU_GPIO_PORT_B
#define BSP_LED0_PIN       MCU_GPIO_PIN_0
#define BSP_LED1_PORT      MCU_GPIO_PORT_B
#define BSP_LED1_PIN       MCU_GPIO_PIN_1

#define BSP_GPIO_GET_PORT(pin) \
    ((pin) == BSP_GPIO_PIN_LED0 ? BSP_LED0_PORT : \
     (pin) == BSP_GPIO_PIN_LED1 ? BSP_LED1_PORT : \
     MCU_GPIO_PORT_A)

#define BSP_GPIO_GET_PIN(pin) \
    ((pin) == BSP_GPIO_PIN_LED0 ? BSP_LED0_PIN : \
     (pin) == BSP_GPIO_PIN_LED1 ? BSP_LED1_PIN : \
     MCU_GPIO_PIN_0)

typedef enum {
    BSP_GPIO_PIN_LED0,
    BSP_GPIO_PIN_LED1
} bsp_gpio_t;

void bsp_gpio_config(bsp_gpio_t pin, const mcu_gpio_config_t *config);
void bsp_gpio_set(bsp_gpio_t pin, mcu_gpio_state_t state);
void bsp_gpio_toggle(bsp_gpio_t pin);

#endif