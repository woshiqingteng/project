#include "bsp_gpio.h"
#include "bsp_led.h"

void bsp_led_set(bsp_led_id_t led_id, bsp_led_state_t state)
{
    bsp_gpio_pin_t pin = (led_id == BSP_LED_0) ? BSP_GPIO_PIN_LED0 : BSP_GPIO_PIN_LED1;
    bsp_gpio_state_t gpio_state = (state == BSP_LED_ON) ? MCU_GPIO_PIN_RESET : MCU_GPIO_PIN_SET;
    bsp_gpio_pin_set(pin, gpio_state);
}

void bsp_led_toggle(bsp_led_id_t led_id)
{
    bsp_gpio_pin_t pin = (led_id == BSP_LED_0) ? BSP_GPIO_PIN_LED0 : BSP_GPIO_PIN_LED1;
    bsp_gpio_pin_toggle(pin);
}

bsp_led_state_t bsp_led_get(bsp_led_id_t led_id)
{
    bsp_gpio_pin_t pin = (led_id == BSP_LED_0) ? BSP_GPIO_PIN_LED0 : BSP_GPIO_PIN_LED1;
    bsp_gpio_state_t gpio_state = bsp_gpio_pin_get(pin);
    return (gpio_state == MCU_GPIO_PIN_RESET) ? BSP_LED_ON : BSP_LED_OFF;
}