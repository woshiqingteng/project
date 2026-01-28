#include "bsp_gpio.h"

void bsp_gpio_config(bsp_gpio_t pin, const mcu_gpio_config_t *config)
{
    mcu_gpio_config_t mcu_config = {
        .mode = config->mode,
        .otype = config->otype,
        .ospeed = config->ospeed,
        .pupd = config->pupd,
        .af = config->af
    };
    mcu_gpio_config(BSP_GPIO_GET_PORT(pin), BSP_GPIO_GET_PIN(pin), &mcu_config);
}

void bsp_gpio_set(bsp_gpio_t pin, mcu_gpio_state_t state)
{
    mcu_gpio_set(BSP_GPIO_GET_PORT(pin), BSP_GPIO_GET_PIN(pin), state);
}

void bsp_gpio_toggle(bsp_gpio_t pin)
{   
    mcu_gpio_toggle(BSP_GPIO_GET_PORT(pin), BSP_GPIO_GET_PIN(pin));
}