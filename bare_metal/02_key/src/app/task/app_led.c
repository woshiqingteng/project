#include "app_led.h"
#include "bsp.h"

void app_led_run(void)
{
    // bsp_led_set(BSP_DS_0, BSP_LED_ON);
    // bsp_led_set(BSP_DS_1, BSP_LED_ON);
    
    bsp_delay_ms(500);
    
    // bsp_led_set(BSP_DS_0, BSP_LED_OFF);
    // bsp_led_set(BSP_DS_1, BSP_LED_OFF);

    bsp_delay_ms(500);
}