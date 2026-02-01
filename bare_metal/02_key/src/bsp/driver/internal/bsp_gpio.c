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

void bsp_gpio_set(bsp_gpio_t pin, mcu_gpio_setting_t setting)
{
    mcu_gpio_set(BSP_GPIO_GET_PORT(pin), BSP_GPIO_GET_PIN(pin), setting);
}

void bsp_gpio_toggle(bsp_gpio_t pin)
{   
    mcu_gpio_toggle(BSP_GPIO_GET_PORT(pin), BSP_GPIO_GET_PIN(pin));
}

mcu_gpio_state_t bsp_gpio_get(bsp_gpio_t pin)
{   
    return mcu_gpio_get(BSP_GPIO_GET_PORT(pin), BSP_GPIO_GET_PIN(pin));
}