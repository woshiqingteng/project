#ifndef BSP_LED_H
#define BSP_LED_H

#include <stdint.h>

typedef enum {
    BSP_LED_0 = 0,
    BSP_LED_1 = 1
} bsp_led_id_t;

typedef enum {
    BSP_LED_OFF = 0,
    BSP_LED_ON = 1
} bsp_led_state_t;

void bsp_led_set(bsp_led_id_t led_id, bsp_led_state_t state);
void bsp_led_toggle(bsp_led_id_t led_id);
bsp_led_state_t bsp_led_get(bsp_led_id_t led_id);

#endif