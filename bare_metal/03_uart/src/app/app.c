#include "app.h"
#include "bsp.h"

void app_init(void)
{
    bsp_init();
}

void app_run(void)
{
    app_key_run();
    // app_led_run();
}