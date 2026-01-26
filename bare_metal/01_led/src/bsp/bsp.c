#include "mcu.h"
#include "bsp_led.h"

static void bsp_init_driver(void)
{
    bsp_led_init();
}

void bsp_init(void)
{
    mcu_init();
    bsp_init_driver();
}