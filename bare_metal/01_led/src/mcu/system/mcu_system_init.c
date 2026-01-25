#include "mcu_system_init.h"
#include "stm32f4xx.h"
#include <stddef.h>

static uint8_t mcu_system_clock_set(uint32_t pll_n, uint32_t pll_m, uint32_t pll_p, uint32_t pll_q);

uint8_t mcu_system_clock_init(const mcu_system_clock_config_t* p_config)
{
    if (p_config == NULL)
    {
        return 1;
    }
    
    return mcu_system_clock_set(p_config->pll_n, p_config->pll_m, 
                           p_config->pll_p, p_config->pll_q);
}

void mcu_system_init(uint32_t pll_n, uint32_t pll_m, uint32_t pll_p, uint32_t pll_q)
{
    RCC->CR |= 0x00000001;           /* Set HSION, enable internal high-speed RC oscillator */
    RCC->CFGR = 0x00000000;          /* Clear CFGR register */
    RCC->CR &= 0xFEF6FFFF;           /* Clear HSEON, CSSON and PLLON bits */
    RCC->PLLCFGR = 0x24003010;       /* Reset PLLCFGR to default value */
    RCC->CR &= ~(1 << 18);           /* Clear HSEBYP, external crystal not bypassed */
    RCC->CIR = 0x00000000;           /* Disable RCC clock interrupts */
    
    mcu_system_clock_set(pll_n, pll_m, pll_p, pll_q);
    
    /* Configure vector table */
#ifdef VECT_TAB_RAM
    mcu_system_set_vector_table(1 << 29, 0x0);
#else
    mcu_system_set_vector_table(0x08000000, 0x0);
#endif
}

void mcu_system_set_vector_table(uint32_t nvic_vect_tab, uint32_t offset)
{
    /* Set NVIC vector table offset register, lower 9 bits reserved */
    SCB->VTOR = nvic_vect_tab | (offset & (uint32_t)0xFFFFFE00);
}

void mcu_system_soft_reset(void)
{
    SCB->AIRCR = 0x05FA0000 | (uint32_t)0x04;
}

void mcu_system_set_stack_pointer(uint32_t addr)
{
    __set_MSP(addr);
}

static uint8_t mcu_system_clock_set(uint32_t pll_n, uint32_t pll_m, 
                               uint32_t pll_p, uint32_t pll_q)
{
    RCC->CR |= 1 << 16;                              /* Enable HSE */
    /* Wait for HSE ready */
    while(!(RCC->CR & (1 << 17)) == 0);
    
    RCC->APB1ENR |= 1 << 28;                     /* Enable power interface clock */
    PWR->CR |= 3 << 14;                          /* High-performance mode, up to 180MHz */
        
    /* HCLK no prescaler, APB1 4 prescaler, APB2 2 prescaler */
    RCC->CFGR |= (0 << 4) | (5 << 10) | (4 << 13);
    RCC->CR &= ~(1 << 24);                       /* Disable main PLL */
    /* Configure main PLL, PLL clock source from HSE */
    RCC->PLLCFGR = pll_m | (pll_n << 6) | (((pll_p >> 1) - 1) << 16) | 
                      (pll_q << 24) | (1 << 22);
        
    RCC->CR |= 1 << 24;                          /* Enable main PLL */
        
    while ((RCC->CR & (1 << 25)) == 0)           /* Wait for PLL ready */
        
    FLASH->ACR |= 1 << 8;                        /* Enable instruction prefetch */
    FLASH->ACR |= 1 << 9;                        /* Enable instruction cache */
    FLASH->ACR |= 1 << 10;                       /* Enable data cache */
    FLASH->ACR |= 5 << 0;                        /* 5 CPU wait cycles */
        
    RCC->CFGR &= ~(3 << 0);                      /* Clear */
    RCC->CFGR |= 2 << 0;                         /* Select main PLL as system clock */
        
    while ((RCC->CFGR & (3 << 2)) != (2 << 2));   /* Wait for PLL as system clock success */

    return 0;
}