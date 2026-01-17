/**
 * @file bsp_rcc.c
 * @brief 时钟配置和延时函数
 */

#include "bsp_rcc.h"
#include "stm32f429xx.h"

/* SysTick计数器 */
static volatile uint32_t s_systick_counter = 0;

/**
 * @brief SysTick中断处理函数
 */
void SysTick_Handler(void)
{
    if (s_systick_counter > 0)
    {
        s_systick_counter--;
    }
}

/**
 * @brief 系统时钟配置
 */
void bsp_rcc_init(void)
{
    /* 启用HSE */
    RCC->CR |= RCC_CR_HSEON;
    while (!(RCC->CR & RCC_CR_HSERDY));
    
    /* 配置PLL: HSE=25MHz → 180MHz */
    RCC->PLLCFGR = (25U << 0U) |      /* PLL_M = 25 */
                   (360U << 6U) |     /* PLL_N = 360 */
                   (0U << 16U) |      /* PLL_P = 2 */
                   (8U << 24U);       /* PLL_Q = 8 */
    
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSE;
    
    /* 启用PLL */
    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY));
    
    /* Flash配置 */
    FLASH->ACR = FLASH_ACR_PRFTEN | 
                 FLASH_ACR_ICEN | 
                 FLASH_ACR_DCEN | 
                 (5U << 0U);  /* 5等待状态 (180MHz) */
    
    /* 总线分频 */
    RCC->CFGR |= (0U << 4U) |    /* AHB = 1 (180MHz) */
                 (5U << 10U) |   /* APB1 = 4 (45MHz) */
                 (4U << 13U);    /* APB2 = 2 (90MHz) */
    
    /* 切换系统时钟 */
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while (!(RCC->CFGR & RCC_CFGR_SWS_PLL));
    
    /* 更新系统时钟变量 */
    SystemCoreClock = 180000000U;
    
    /* 初始化SysTick (1ms中断) */
    SysTick_Config(SystemCoreClock / 1000U);
}

/**
 * @brief 毫秒延时
 */
void bsp_delay_ms(uint32_t ms)
{
    s_systick_counter = ms;
    while (s_systick_counter > 0)
    {
        /* 等待SysTick中断 */
        __WFI();  /* 等待中断，降低功耗 */
    }
}