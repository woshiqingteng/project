#ifndef BSP_GPIO_H
#define BSP_GPIO_H

#include "mcu_gpio.h"

// led
#define BSP_GPIO_PORT_LED0      MCU_GPIO_PORT_B
#define BSP_GPIO_PIN_LED0       MCU_GPIO_PIN_0
#define BSP_GPIO_PORT_LED1      MCU_GPIO_PORT_B
#define BSP_GPIO_PIN_LED1       MCU_GPIO_PIN_1
// key
#define BSP_GPIO_PORT_KEY0      MCU_GPIO_PORT_H
#define BSP_GPIO_PIN_KEY0       MCU_GPIO_PIN_3
#define BSP_GPIO_PORT_KEY1      MCU_GPIO_PORT_H
#define BSP_GPIO_PIN_KEY1       MCU_GPIO_PIN_2
#define BSP_GPIO_PORT_KEY2      MCU_GPIO_PORT_C
#define BSP_GPIO_PIN_KEY2       MCU_GPIO_PIN_13
#define BSP_GPIO_PORT_WKUP      MCU_GPIO_PORT_A
#define BSP_GPIO_PIN_WKUP       MCU_GPIO_PIN_0

#define BSP_GPIO_GET_PORT(pin) \
    ((pin) == BSP_GPIO_LED0 ? BSP_GPIO_PORT_LED0 : \
     (pin) == BSP_GPIO_LED1 ? BSP_GPIO_PORT_LED1 : \
     (pin) == BSP_GPIO_KEY0 ? BSP_GPIO_PORT_KEY0 : \
     (pin) == BSP_GPIO_KEY1 ? BSP_GPIO_PORT_KEY1 : \
     (pin) == BSP_GPIO_KEY2 ? BSP_GPIO_PORT_KEY2 : \
     (pin) == BSP_GPIO_WKUP ? BSP_GPIO_PORT_WKUP : \
     MCU_GPIO_PORT_A)

#define BSP_GPIO_GET_PIN(pin) \
    ((pin) == BSP_GPIO_LED0 ? BSP_GPIO_PIN_LED0 : \
     (pin) == BSP_GPIO_LED1 ? BSP_GPIO_PIN_LED1 : \
     (pin) == BSP_GPIO_KEY0 ? BSP_GPIO_PIN_KEY0 : \
     (pin) == BSP_GPIO_KEY1 ? BSP_GPIO_PIN_KEY1 : \
     (pin) == BSP_GPIO_KEY2 ? BSP_GPIO_PIN_KEY2 : \
     (pin) == BSP_GPIO_WKUP ? BSP_GPIO_PIN_WKUP : \
     MCU_GPIO_PIN_0)

typedef enum {
    BSP_GPIO_LED0,
    BSP_GPIO_LED1,
    BSP_GPIO_KEY0,
    BSP_GPIO_KEY1,
    BSP_GPIO_KEY2,
    BSP_GPIO_WKUP
} bsp_gpio_t;

void bsp_gpio_config(bsp_gpio_t pin, const mcu_gpio_config_t *config);
void bsp_gpio_set(bsp_gpio_t pin, mcu_gpio_setting_t setting);
void bsp_gpio_toggle(bsp_gpio_t pin);
mcu_gpio_state_t bsp_gpio_get(bsp_gpio_t pin);

#endif