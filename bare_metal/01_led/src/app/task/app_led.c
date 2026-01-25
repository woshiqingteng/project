#include "bsp_led.h"
#include "bsp_delay.h"

void app_led_run(void)
{
    bsp_led_set(BSP_LED_0, BSP_LED_ON);
    bsp_led_set(BSP_LED_1, BSP_LED_OFF);
    
    bsp_delay_ms(500);
    
    bsp_led_set(BSP_LED_0, BSP_LED_OFF);
    bsp_led_set(BSP_LED_1, BSP_LED_ON);

    bsp_delay_ms(500);
}