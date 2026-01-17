/**
 * @file app_led.c
 * @brief LED应用
 */

#include "app_interface.h"
#include "app_led.h"
#include "bsp_interface.h"

/* 私有变量 */
static uint8_t led_mode = LED_MODE_ALTERNATE;
static uint32_t tick_counter = 0;

/**
 * @brief 应用初始化
 */
void app_init(void)
{
    /* 板级初始化 */
    bsp_init();
    
    /* LED初始状态 */
    led_mode = LED_MODE_ALTERNATE;
    tick_counter = 0;
}

/**
 * @brief 应用主循环
 */
void app_run(void)
{
    tick_counter++;
    
    switch (led_mode)
    {
        case LED_MODE_OFF:
            bsp_led_set(LED0, 0);
            bsp_led_set(LED1, 0);
            break;
            
        case LED_MODE_ON:
            bsp_led_set(LED0, 1);
            bsp_led_set(LED1, 1);
            break;
            
        case LED_MODE_SLOW_BLINK:
            if (tick_counter % 1000 < 500)
            {
                bsp_led_set(LED0, 1);
                bsp_led_set(LED1, 1);
            }
            else
            {
                bsp_led_set(LED0, 0);
                bsp_led_set(LED1, 0);
            }
            break;
            
        case LED_MODE_FAST_BLINK:
            if (tick_counter % 200 < 100)
            {
                bsp_led_set(LED0, 1);
                bsp_led_set(LED1, 1);
            }
            else
            {
                bsp_led_set(LED0, 0);
                bsp_led_set(LED1, 0);
            }
            break;
            
        case LED_MODE_ALTERNATE:
            if (tick_counter % 600 < 300)
            {
                bsp_led_set(LED0, 1);
                bsp_led_set(LED1, 0);
            }
            else
            {
                bsp_led_set(LED0, 0);
                bsp_led_set(LED1, 1);
            }
            break;
    }
    
    /* 简单延时 */
    for (volatile uint32_t i = 0; i < 100000; i++);
}

/**
 * @brief 设置LED模式
 */
void app_led_set_mode(uint8_t mode)
{
    if (mode < LED_MODE_MAX)
    {
        led_mode = mode;
        tick_counter = 0;
    }
}