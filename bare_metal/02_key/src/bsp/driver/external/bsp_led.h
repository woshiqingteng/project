#ifndef BSP_LED_H
#define BSP_LED_H

#include "bsp_gpio.h"

typedef bsp_gpio_t bsp_led_id_t;
typedef mcu_gpio_setting_t bsp_led_setting_t;
typedef mcu_gpio_state_t bsp_led_state_t;

#define BSP_DS_0 BSP_GPIO_LED1
#define BSP_DS_1 BSP_GPIO_LED0
#define BSP_LED_ON MCU_GPIO_PIN_RESET
#define BSP_LED_OFF MCU_GPIO_PIN_SET
#define BSP_LED_IS_ON MCU_GPIO_LOW
#define BSP_LED_IS_OFF MCU_GPIO_HIGH

void bsp_led_init(void);
void bsp_led_set(bsp_led_id_t led_id, bsp_led_setting_t setting);
void bsp_led_toggle(bsp_led_id_t led_id);
bsp_led_state_t bsp_led_get(bsp_led_id_t led_id);

#endif