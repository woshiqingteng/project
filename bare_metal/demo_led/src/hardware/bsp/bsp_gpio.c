/**
 * @file bsp_gpio.c
 * @brief GPIO驱动
 */

#include "bsp_interface.h"
#include "bsp_gpio.h"
#include "bsp_rcc.h"  /* 添加这行：包含bsp_rcc.h头文件 */
#include "stm32f429xx.h"

/* LED引脚定义 */
#define LED0_PIN     GPIO_PIN_1
#define LED0_PORT    GPIOB
#define LED0_CLK     RCC_AHB1ENR_GPIOBEN

#define LED1_PIN     GPIO_PIN_0
#define LED1_PORT    GPIOB

/**
 * @brief 初始化LED GPIO
 */
static void gpio_led_init(void)
{
    /* 启用GPIOB时钟 */
    RCC->AHB1ENR |= LED0_CLK;
    
    /* 配置LED0 (PB1) */
    LED0_PORT->MODER &= ~(3U << (1U * 2U));
    LED0_PORT->MODER |= (1U << (1U * 2U));
    
    LED0_PORT->OTYPER &= ~(1U << 1U);
    LED0_PORT->OSPEEDR |= (3U << (1U * 2U));
    
    /* 配置LED1 (PB0) */
    LED1_PORT->MODER &= ~(3U << (0U * 2U));
    LED1_PORT->MODER |= (1U << (0U * 2U));
    
    LED1_PORT->OTYPER &= ~(1U << 0U);
    LED1_PORT->OSPEEDR |= (3U << (0U * 2U));
    
    /* 初始关闭 */
    LED0_PORT->ODR &= ~LED0_PIN;
    LED1_PORT->ODR &= ~LED1_PIN;
}

/**
 * @brief 板级初始化
 */
void bsp_init(void)
{
    /* 初始化时钟 */
    bsp_rcc_init();  /* 这里调用了bsp_rcc_init函数 */
    
    /* 初始化GPIO */
    gpio_led_init();
}

/**
 * @brief 设置LED状态
 */
void bsp_led_set(uint8_t led_id, uint8_t state)
{
    switch (led_id)
    {
        case LED0:
            if (state)
            {
                LED0_PORT->ODR |= LED0_PIN;
            }
            else
            {
                LED0_PORT->ODR &= ~LED0_PIN;
            }
            break;
            
        case LED1:
            if (state)
            {
                LED1_PORT->ODR |= LED1_PIN;
            }
            else
            {
                LED1_PORT->ODR &= ~LED1_PIN;
            }
            break;
    }
}

/**
 * @brief 切换LED状态
 */
void bsp_led_toggle(uint8_t led_id)
{
    switch (led_id)
    {
        case LED0:
            LED0_PORT->ODR ^= LED0_PIN;
            break;
            
        case LED1:
            LED1_PORT->ODR ^= LED1_PIN;
            break;
    }
}