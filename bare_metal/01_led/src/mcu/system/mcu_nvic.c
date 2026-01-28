#include "mcu_nvic.h"
#include "stm32f4xx.h"

// core_cm4.h NVIC_SetPriorityGrouping
void mcu_nvic_prio_group_cfg(uint8_t nvic_group)
{
    uint32_t temp = 0;
    uint32_t temp1 = 0;
    
    temp1 = (~nvic_group) & 0x07;
    temp1 <<= 8;
    temp = SCB->AIRCR;
    temp &= 0x0000F8FF;
    temp |= 0x05FA0000;
    temp |= temp1;
    SCB->AIRCR = temp;
}

// core_cm4.h NVIC_GetPriorityGrouping
void mcu_nvic_init(uint8_t preemption_priority, uint8_t sub_priority,
               uint8_t channel, uint8_t group)
{
    uint32_t temp = 0;
    
    mcu_nvic_prio_group_cfg(group);
    
    temp = preemption_priority << (4 - group);
    temp |= sub_priority & (0x0F >> group);
    temp &= 0xF;
    
    NVIC->ISER[channel / 32] |= 1 << (channel % 32);
    NVIC->IP[channel] |= temp << 4;
}

// stm32f4xx_hal/ll_exti.c/h
void mcu_nvic_exti_config(uint8_t gpio_x, uint8_t bit_x, uint8_t trim)
{
    uint8_t ext_offset = (bit_x % 4) * 4;
    
    RCC->APB2ENR |= 1 << 14;
    SYSCFG->EXTICR[bit_x / 4] &= ~(0x000F << ext_offset);
    SYSCFG->EXTICR[bit_x / 4] |= gpio_x << ext_offset;
    EXTI->IMR |= 1 << bit_x;
    
    if (trim & 0x01)
    {
        EXTI->FTSR |= 1 << bit_x;
    }
    
    if (trim & 0x02)
    {
        EXTI->RTSR |= 1 << bit_x;
    }
}