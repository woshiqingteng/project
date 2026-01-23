#include "mcu_init.h"

void mcu_init_system(void)
{
    mcu_system_init(360, 25, 2, 8);
    // mcu_systick_init(22500);
    mcu_systick_config(180);
}

void mcu_init_peripheral(void)
{
    mcu_gpio_en();
}

void mcu_init(void)
{
    mcu_init_system();
    mcu_init_peripheral();
}