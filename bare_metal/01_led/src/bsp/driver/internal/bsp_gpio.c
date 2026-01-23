#include "bsp_gpio.h"
#include "bsp_gpio_pin.h"

bool bsp_gpio_pin_config(bsp_gpio_pin_t pin, const bsp_gpio_config_t *config)
{
    if (!BSP_GPIO_IS_VALID_PIN(pin) || !config) return false;
    
    mcu_gpio_clk_en_port(BSP_GPIO_GET_MCU_PORT(pin));
    mcu_gpio_pin_config(BSP_GPIO_GET_MCU_PORT(pin), BSP_GPIO_GET_MCU_PIN(pin), 
                       (const mcu_gpio_config_t *)config);
    return true;
}

void bsp_gpio_pin_set(bsp_gpio_pin_t pin, bsp_gpio_state_t state)
{
    if (!BSP_GPIO_IS_VALID_PIN(pin)) return;
    
    mcu_gpio_pin_write(BSP_GPIO_GET_MCU_PORT(pin), BSP_GPIO_GET_MCU_PIN(pin), state);
}

bsp_gpio_state_t bsp_gpio_pin_get(bsp_gpio_pin_t pin)
{
    if (!BSP_GPIO_IS_VALID_PIN(pin)) return MCU_GPIO_PIN_RESET;
    
    return mcu_gpio_pin_read(BSP_GPIO_GET_MCU_PORT(pin), BSP_GPIO_GET_MCU_PIN(pin));
}

void bsp_gpio_pin_toggle(bsp_gpio_pin_t pin)
{
    if (!BSP_GPIO_IS_VALID_PIN(pin)) return;
    mcu_gpio_pin_toggle(BSP_GPIO_GET_MCU_PORT(pin), BSP_GPIO_GET_MCU_PIN(pin));
}