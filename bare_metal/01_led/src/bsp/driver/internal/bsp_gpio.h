#ifndef BSP_GPIO_H
#define BSP_GPIO_H

// #include <stdbool.h>
// #include <stdint.h>
#include "mcu_gpio.h"

typedef enum {
    BSP_GPIO_PIN_LED0,
    BSP_GPIO_PIN_LED1,
    BSP_GPIO_PIN_COUNT
} bsp_gpio_pin_t;

typedef mcu_gpio_pin_state_t bsp_gpio_state_t;

typedef struct {
    mcu_gpio_mode_t mode;
    mcu_gpio_otype_t otype;
    mcu_gpio_ospeed_t ospeed;
    mcu_gpio_pupd_t pupd;
} bsp_gpio_config_t;

bool bsp_gpio_pin_config(bsp_gpio_pin_t pin, const bsp_gpio_config_t *config);
void bsp_gpio_pin_set(bsp_gpio_pin_t pin, bsp_gpio_state_t state);
bsp_gpio_state_t bsp_gpio_pin_get(bsp_gpio_pin_t pin);
void bsp_gpio_pin_toggle(bsp_gpio_pin_t pin);

#endif