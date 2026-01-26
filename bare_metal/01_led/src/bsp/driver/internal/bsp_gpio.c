#include "bsp_gpio.h"
#include <stdbool.h>

bool bsp_gpio_pin_config(bsp_gpio_pin_t pin, const bsp_gpio_config_t *config)
{
    if (!config) return false;
    if (pin >= BSP_GPIO_PIN_COUNT) return false;
    
    mcu_gpio_config_t mcu_config = {
        .mode = config->mode,
        .otype = config->otype,
        .ospeed = config->ospeed,
        .pupd = config->pupd,
        .af = MCU_GPIO_AF0  // 默认值，如果不是复用模式则不会使用
    };
    
    mcu_gpio_pin_config(BSP_GPIO_GET_MCU_PORT(pin), 
                       BSP_GPIO_GET_MCU_PIN(pin), 
                       &mcu_config);
    return true;
}

bool bsp_gpio_pin_af_config(bsp_gpio_pin_t pin, mcu_gpio_af_t af)
{
    if (pin >= BSP_GPIO_PIN_COUNT) return false;
    
    mcu_gpio_pin_af_config(BSP_GPIO_GET_MCU_PORT(pin), 
                          BSP_GPIO_GET_MCU_PIN(pin), 
                          af);
    return true;
}

void bsp_gpio_pin_set(bsp_gpio_pin_t pin, bsp_gpio_state_t state)
{
    if (pin >= BSP_GPIO_PIN_COUNT) return;
    
    mcu_gpio_pin_write(BSP_GPIO_GET_MCU_PORT(pin), 
                      BSP_GPIO_GET_MCU_PIN(pin), 
                      state);
}

bsp_gpio_state_t bsp_gpio_pin_get(bsp_gpio_pin_t pin)
{
    if (pin >= BSP_GPIO_PIN_COUNT) return MCU_GPIO_PIN_RESET;
    
    return mcu_gpio_pin_read(BSP_GPIO_GET_MCU_PORT(pin), 
                            BSP_GPIO_GET_MCU_PIN(pin));
}

void bsp_gpio_pin_toggle(bsp_gpio_pin_t pin)
{
    if (pin >= BSP_GPIO_PIN_COUNT) return;
    
    mcu_gpio_pin_toggle(BSP_GPIO_GET_MCU_PORT(pin), 
                       BSP_GPIO_GET_MCU_PIN(pin));
}