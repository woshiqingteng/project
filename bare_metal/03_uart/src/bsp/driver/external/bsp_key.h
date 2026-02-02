#ifndef BSP_KEY_H
#define BSP_KEY_H

#include "bsp_gpio.h"

#define BSP_KEY_0 BSP_GPIO_KEY0
#define BSP_KEY_1 BSP_GPIO_KEY1
#define BSP_KEY_2 BSP_GPIO_KEY2
#define BSP_WK_UP BSP_GPIO_WKUP
#define BSP_KEY_ON MCU_GPIO_HIGH
#define BSP_KEY_OFF MCU_GPIO_LOW

typedef enum {
    BSP_KEY_SINGLE = 0,
    BSP_KEY_CONTINUE = 1
} bsp_key_mode_t;

typedef enum {
    BSP_KEY_NONE = 0,
    BSP_KEY_0_PRESS = 1,
    BSP_KEY_1_PRESS = 2,
    BSP_KEY_2_PRESS = 3,
    BSP_KEY_WK_UP_PRESS = 4
} bsp_key_state_t;

void bsp_key_init(void);
bsp_key_state_t bsp_key_scan(bsp_key_mode_t mode);

#endif