#include "mcu_system_init.h"
#include "stm32f4xx.h"

static uint8_t mcu_system_clock_set(uint32_t pll_n, uint32_t pll_m, uint32_t pll_p, uint32_t pll_q);

void mcu_system_init(uint32_t pll_n, uint32_t pll_m, uint32_t pll_p, uint32_t pll_q)
{
    RCC->CR |= 0x00000001;
    RCC->CFGR = 0x00000000;
    RCC->CR &= 0xFEF6FFFF;
    RCC->PLLCFGR = 0x24003010;
    RCC->CR &= ~(1 << 18);
    RCC->CIR = 0x00000000;
    
    mcu_system_clock_set(pll_n, pll_m, pll_p, pll_q);

#ifdef VECT_TAB_RAM
    mcu_system_set_vector_table(1 << 29, 0x0);
#else
    mcu_system_set_vector_table(0x08000000, 0x0);
#endif
}

void mcu_system_set_vector_table(uint32_t nvic_vect_tab, uint32_t offset)
{
    SCB->VTOR = nvic_vect_tab | (offset & (uint32_t)0xFFFFFE00);
}

// core_cm4.h NVIC_SystemReset
void mcu_system_soft_reset(void)
{
    SCB->AIRCR = 0x05FA0000 | (uint32_t)0x04;
}

void mcu_system_set_stack_pointer(uint32_t addr)
{
    __set_MSP(addr);
}

// `stm32f4xx_hal/ll_rcc.c/h` `stm32f4xx_hal/ll_bus.c/h`
static uint8_t mcu_system_clock_set(uint32_t pll_n, uint32_t pll_m, 
                               uint32_t pll_p, uint32_t pll_q)
{
    RCC->CR |= 1 << 16;
    while(!(RCC->CR & (1 << 17)) == 0);
    
    RCC->APB1ENR |= 1 << 28;
    PWR->CR |= 3 << 14;
    RCC->CFGR |= (0 << 4) | (5 << 10) | (4 << 13);
    RCC->CR &= ~(1 << 24);
    RCC->PLLCFGR = pll_m | (pll_n << 6) | (((pll_p >> 1) - 1) << 16) | 
                      (pll_q << 24) | (1 << 22);
        
    RCC->CR |= 1 << 24;
        
    while ((RCC->CR & (1 << 25)) == 0)
        
    FLASH->ACR |= 1 << 8;
    FLASH->ACR |= 1 << 9;
    FLASH->ACR |= 1 << 10;
    FLASH->ACR |= 5 << 0;
        
    RCC->CFGR &= ~(3 << 0);
    RCC->CFGR |= 2 << 0;
        
    while ((RCC->CFGR & (3 << 2)) != (2 << 2));

    return 0;
}