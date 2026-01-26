#include "bsp_led.h"
#include "bsp_gpio.h"
#include <stdbool.h>

/* LED默认配置 */
static const bsp_gpio_config_t led_config = {
    .mode = MCU_GPIO_MODE_OUTPUT,
    .otype = MCU_GPIO_OTYPE_PUSHPULL,
    .ospeed = MCU_GPIO_OSPEED_HIGH,
    .pupd = MCU_GPIO_PUPD_NONE
};

bool bsp_led_init(void)
{
    bool success = true;
    
    /* 配置所有LED引脚 */
    for (int i = 0; i < BSP_LED_COUNT; i++) {
        bsp_gpio_pin_t pin = BSP_LED_GET_PIN(i);
        
        if (!bsp_gpio_pin_config(pin, &led_config)) {
            success = false;
        }
        
        /* 初始状态：关闭LED（假设LED是低电平点亮） */
        bsp_gpio_pin_set(pin, MCU_GPIO_PIN_SET);
    }
    
    return success;
}

bool bsp_led_set(bsp_led_id_t led_id, bsp_led_state_t state)
{
    if (led_id >= BSP_LED_COUNT) return false;
    
    bsp_gpio_pin_t pin = BSP_LED_GET_PIN(led_id);
    
    /* 假设LED是低电平点亮，所以ON对应RESET，OFF对应SET */
    bsp_gpio_state_t gpio_state = (state == BSP_LED_ON) ? 
                                  MCU_GPIO_PIN_RESET : 
                                  MCU_GPIO_PIN_SET;
    
    bsp_gpio_pin_set(pin, gpio_state);
    return true;
}

bool bsp_led_toggle(bsp_led_id_t led_id)
{
    if (led_id >= BSP_LED_COUNT) return false;
    
    bsp_gpio_pin_t pin = BSP_LED_GET_PIN(led_id);
    bsp_gpio_pin_toggle(pin);
    return true;
}

bool bsp_led_get(bsp_led_id_t led_id, bsp_led_state_t *state)
{
    if (led_id >= BSP_LED_COUNT || !state) return false;
    
    bsp_gpio_pin_t pin = BSP_LED_GET_PIN(led_id);
    bsp_gpio_state_t gpio_state = bsp_gpio_pin_get(pin);
    
    /* 假设LED是低电平点亮，所以GPIO低电平对应LED ON */
    *state = (gpio_state == MCU_GPIO_PIN_RESET) ? 
             BSP_LED_ON : 
             BSP_LED_OFF;
    
    return true;
}