#include "mcu.h"
#include "bsp_delay.h"

void bsp_delay_us(uint32_t us)
{
    mcu_systick_delay_us(us);
}

void bsp_delay_ms(uint16_t ms)
{
    mcu_systick_delay_ms(ms);
}