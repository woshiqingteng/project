#include "mcu_systick.h"
#include "stm32f4xx.h"

static void mcu_systick_delay_xms(uint16_t nms);

static uint8_t s_fac_us = 0;
static uint16_t s_fac_ms = 0;

// core_cm4.h SysTick_Config `stm32f429xx.h` `__Vendor_SysTickConfig`
void mcu_systick_config(uint32_t sysclk)
{
    SysTick->CTRL &= ~(1 << 2);
    
    s_fac_us = sysclk / 8;
    s_fac_ms = (uint16_t)(sysclk * 1000) / 8;
}

void mcu_systick_delay_us(uint32_t us)
{
    uint32_t temp = 0;
    

    SysTick->LOAD = us * s_fac_us;
    SysTick->VAL = 0x00;
    SysTick->CTRL = 0x01;
    
    do
    {
        temp = SysTick->CTRL;
    }
    while ((temp & 0x01) && !(temp & (1 << 16)));
    
    SysTick->CTRL = 0x00;
    SysTick->VAL = 0x00;
}

static void mcu_systick_delay_xms(uint16_t nms)
{
    uint32_t temp = 0;
    
    SysTick->LOAD = (uint32_t)nms * s_fac_ms;
    SysTick->VAL = 0x00;
    SysTick->CTRL = 0x01;
    
    do
    {
        temp = SysTick->CTRL;
    }
    while ((temp & 0x01) && !(temp & (1 << 16)));
    
    SysTick->CTRL = 0x00;
    SysTick->VAL = 0x00;
}

void mcu_systick_delay_ms(uint16_t ms)
{
    uint8_t repeat = ms / 540;
    uint16_t remain = ms % 540;

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