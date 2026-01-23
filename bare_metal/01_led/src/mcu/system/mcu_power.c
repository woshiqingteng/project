#include "mcu_power.h"

void mcu_power_enter_standby(void)
{
    SCB->SCR |= 1 << 2;                              /* Enable SLEEPDEEP bit */
    RCC->APB1ENR |= 1 << 28;                         /* Enable power clock */
    PWR->CSR |= 1 << 8;                              /* Set WKUP for wakeup */
    PWR->CR |= 1 << 2;                               /* Clear wake-up flag */
    PWR->CR |= 1 << 1;                               /* Set PDDS bit */
    mcu_power_wait_for_interrupt();                      /* Execute WFI instruction */
}

void mcu_power_wait_for_interrupt(void)
{
    __asm volatile("wfi");
}

void mcu_power_disable_interrupt(void)
{
    __disable_irq();
}

void mcu_power_enable_interrupt(void)
{
    __enable_irq();
}