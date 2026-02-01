#include "app_key.h"
#include "bsp.h"
#include <stdint.h>

void app_key_run(void)
{    
    uint8_t key;
    
    key = bsp_key_scan(BSP_KEY_SINGLE);
    if(key)
    {
        switch(key)
        {
            case BSP_KEY_WK_UP_PRESS:
                if(bsp_led_get(BSP_DS_0) == BSP_LED_IS_ON)
                {
                    bsp_led_set(BSP_DS_0, BSP_LED_OFF);
                    bsp_led_set(BSP_DS_1, BSP_LED_ON);
                }
                else
                {
                    bsp_led_set(BSP_DS_0, BSP_LED_ON);
                    bsp_led_set(BSP_DS_1, BSP_LED_OFF);
                }
                break;
           case BSP_KEY_2_PRESS:
                bsp_led_toggle(BSP_DS_0);
                break;
            case BSP_KEY_1_PRESS:
                bsp_led_toggle(BSP_DS_1);
                break;
            case BSP_KEY_0_PRESS:
                bsp_led_toggle(BSP_DS_0);
                bsp_led_toggle(BSP_DS_1);
                break;
        }
    }
    else
    {
        bsp_delay_ms(10);
    }
}