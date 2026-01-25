#include "bsp.h"
#include "app_led.h"

void app_init(void)
{
    bsp_init();
}

void app_run(void)
{
    app_led_run();
}