#include "mcu_nvic.h"

void mcu_nvic_prio_group_cfg(uint8_t nvic_group)
{
    uint32_t temp = 0;
    uint32_t temp1 = 0;
    
    temp1 = (~nvic_group) & 0x07;    /* Get lower 3 bits */
    temp1 <<= 8;
    temp = SCB->AIRCR;               /* Read current setting */
    temp &= 0x0000F8FF;              /* Clear previous grouping */
    temp |= 0x05FA0000;              /* Write key */
    temp |= temp1;
    SCB->AIRCR = temp;               /* Set grouping */
}

void mcu_nvic_init(uint8_t preemption_priority, uint8_t sub_priority,
               uint8_t channel, uint8_t group)
{
    uint32_t temp = 0;
    
    mcu_nvic_prio_group_cfg(group);    /* Set group */
    
    temp = preemption_priority << (4 - group);
    temp |= sub_priority & (0x0F >> group);
    temp &= 0xF;                                     /* Get lower 4 bits */
    
    NVIC->ISER[channel / 32] |= 1 << (channel % 32); /* Enable interrupt */
    NVIC->IP[channel] |= temp << 4;                  /* Set priority */
}

void mcu_nvic_exti_config(uint8_t gpio_x, uint8_t bit_x, uint8_t trim)
{
    uint8_t ext_offset = (bit_x % 4) * 4;
    
    RCC->APB2ENR |= 1 << 14;                         /* Enable SYSCFG clock */
    
    /* Clear previous setting */
    SYSCFG->EXTICR[bit_x / 4] &= ~(0x000F << ext_offset);
    /* Map EXTI.bit_x to GPIOx.bit_x */
    SYSCFG->EXTICR[bit_x / 4] |= gpio_x << ext_offset;
    
    /* Automatic configuration */
    EXTI->IMR |= 1 << bit_x;                         /* Enable line bit_x interrupt */
    
    if (trim & 0x01)
    {
        EXTI->FTSR |= 1 << bit_x;                    /* Falling edge trigger */
    }
    
    if (trim & 0x02)
    {
        EXTI->RTSR |= 1 << bit_x;                    /* Rising edge trigger */
    }
}