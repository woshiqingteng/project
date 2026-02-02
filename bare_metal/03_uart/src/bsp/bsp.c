#include "mcu.h"
#include "bsp_led.h"
#include "bsp_key.h"

static void bsp_init_driver(void)
{
    bsp_led_init();
    bsp_key_init();
}

void bsp_init(void)
{
    mcu_init();
    bsp_init_driver();
}