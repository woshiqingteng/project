#include "bsp_led.h"

static const mcu_gpio_config_t led_config = {
    .mode = MCU_GPIO_MODE_OUTPUT,
    .otype = MCU_GPIO_OTYPE_PUSHPULL,
    .ospeed = MCU_GPIO_OSPEED_HIGH,
    .pupd = MCU_GPIO_PUPD_UP
};

void bsp_led_init(void)
{      
    bsp_gpio_config(BSP_DS_0, &led_config);
    bsp_gpio_config(BSP_DS_1, &led_config);
}

void bsp_led_set(bsp_led_id_t led_id, bsp_led_state_t state)
{
    bsp_gpio_set(led_id, state);
}

void bsp_led_toggle(bsp_led_id_t led_id)
{
    bsp_gpio_toggle(led_id);
}