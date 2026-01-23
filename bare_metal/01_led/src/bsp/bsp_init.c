#include "mcu.h"
#include "bsp.h"

static void bsp_init_driver(void)
{
    // empty
}

void bsp_init(void)
{
    mcu_init();
    bsp_init_driver();
}