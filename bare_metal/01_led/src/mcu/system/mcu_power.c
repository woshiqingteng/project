#include "mcu_power.h"
#include "stm32f4xx.h"

void mcu_power_enter_standby(void)
{
    SCB->SCR |= 1 << 2;
    RCC->APB1ENR |= 1 << 28;
    PWR->CSR |= 1 << 8;
    PWR->CR |= 1 << 2;
    PWR->CR |= 1 << 1;
    mcu_power_wait_for_interrupt();
}

void mcu_power_wait_for_interrupt(void)
{
    __WFI();
}

void mcu_power_disable_interrupt(void)
{
    __disable_irq();
}

void mcu_power_enable_interrupt(void)
{
    __enable_irq();
}