#include "mcu_systick.h"
#include "stm32f4xx.h"

static void mcu_systick_delay_xms(uint16_t nms);

static uint8_t s_fac_us = 0;
static uint16_t s_fac_ms = 0;

// core_cm4.h SysTick_Config `stm32f429xx.h` `__Vendor_SysTickConfig`
void mcu_systick_config(uint32_t sysclk)
{
    /* SysTick uses external clock source (AHB/8) */
    SysTick->CTRL &= ~(1 << 2);
    
    s_fac_us = sysclk / 8;    /* For non-OS mode */
    s_fac_ms = (uint16_t)(sysclk * 1000) / 8;  /* For non-OS mode */
}

void mcu_systick_delay_us(uint32_t us)
{
    uint32_t temp = 0;
    
    /* Note: nus value should not exceed 798915us (max 2^24/fac_us @ fac_us=21) */
    SysTick->LOAD = us * s_fac_us;  /* Time loading */
    SysTick->VAL = 0x00;           /* Clear counter */
    SysTick->CTRL = 0x01;          /* Start counting down */
    
    do
    {
        temp = SysTick->CTRL;
    }
    while ((temp & 0x01) && !(temp & (1 << 16)));  /* Wait for time to arrive */
    
    SysTick->CTRL = 0x00;          /* Turn off counter */
    SysTick->VAL = 0x00;           /* Clear counter */
}

static void mcu_systick_delay_xms(uint16_t nms)
{
    uint32_t temp = 0;
    
    SysTick->LOAD = (uint32_t)nms * s_fac_ms;  /* Time loading (24-bit) */
    SysTick->VAL = 0x00;                      /* Clear counter */
    SysTick->CTRL = 0x01;                     /* Start counting down */
    
    do
    {
        temp = SysTick->CTRL;
    }
    while ((temp & 0x01) && !(temp & (1 << 16)));  /* Wait for time to arrive */
    
    SysTick->CTRL = 0x00;          /* Turn off counter */
    SysTick->VAL = 0x00;           /* Clear counter */
}

void mcu_systick_delay_ms(uint16_t ms)
{
    uint8_t repeat = ms / 540;     /* Considering possible overclocking */
    uint16_t remain = ms % 540;
    
    /* For 180MHz: delay_xms max can only delay about 541ms */
    while (repeat)
    {
        mcu_systick_delay_xms(540);
        repeat--;
    }
    
    if (remain)
    {
        mcu_systick_delay_xms(remain);
    }
}