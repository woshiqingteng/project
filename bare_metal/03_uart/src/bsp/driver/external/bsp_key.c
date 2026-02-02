#include "bsp_key.h"
#include "bsp_delay.h"

static const mcu_gpio_config_t key0_config = {
    .mode = MCU_GPIO_MODE_INPUT,
    .otype = MCU_GPIO_OTYPE_PUSHPULL,
    .ospeed = MCU_GPIO_OSPEED_LOW,
    .pupd = MCU_GPIO_PUPD_UP
};

static const mcu_gpio_config_t key1_config = {
    .mode = MCU_GPIO_MODE_INPUT,
    .otype = MCU_GPIO_OTYPE_PUSHPULL,
    .ospeed = MCU_GPIO_OSPEED_LOW,
    .pupd = MCU_GPIO_PUPD_UP
};

static const mcu_gpio_config_t key2_config = {
    .mode = MCU_GPIO_MODE_INPUT,
    .otype = MCU_GPIO_OTYPE_PUSHPULL,
    .ospeed = MCU_GPIO_OSPEED_LOW,
    .pupd = MCU_GPIO_PUPD_UP
};

static const mcu_gpio_config_t wkup_config = {
    .mode = MCU_GPIO_MODE_INPUT,
    .otype = MCU_GPIO_OTYPE_PUSHPULL,
    .ospeed = MCU_GPIO_OSPEED_LOW,
    .pupd = MCU_GPIO_PUPD_DOWN
};

void bsp_key_init(void)
{      
    bsp_gpio_config(BSP_KEY_0, &key0_config);
    bsp_gpio_config(BSP_KEY_1, &key1_config);
    bsp_gpio_config(BSP_KEY_2, &key2_config);
    bsp_gpio_config(BSP_WK_UP, &wkup_config);
}

bsp_key_state_t bsp_key_scan(bsp_key_mode_t mode)
{
    static uint8_t key_up = 1;
    
    if (mode == BSP_KEY_CONTINUE) {
        key_up = 1;
    }
    
    if (key_up && (bsp_gpio_get(BSP_KEY_0) == BSP_KEY_OFF || 
                   bsp_gpio_get(BSP_KEY_1) == BSP_KEY_OFF ||
                   bsp_gpio_get(BSP_KEY_2) == BSP_KEY_OFF ||
                   bsp_gpio_get(BSP_WK_UP) == BSP_KEY_ON)) {
        key_up = 0;
        
        if (bsp_gpio_get(BSP_KEY_0) == BSP_KEY_OFF) {
            return BSP_KEY_0_PRESS;
        } else if (bsp_gpio_get(BSP_KEY_1) == BSP_KEY_OFF) {
            return BSP_KEY_1_PRESS;
        } else if (bsp_gpio_get(BSP_KEY_2) == BSP_KEY_OFF) {
            return BSP_KEY_2_PRESS;
        } else if (bsp_gpio_get(BSP_WK_UP) == BSP_KEY_ON) {
            return BSP_KEY_WK_UP_PRESS;
        }
    } else if (bsp_gpio_get(BSP_KEY_0) == BSP_KEY_ON &&
               bsp_gpio_get(BSP_KEY_1) == BSP_KEY_ON &&
               bsp_gpio_get(BSP_KEY_2) == BSP_KEY_ON &&
               bsp_gpio_get(BSP_WK_UP) == BSP_KEY_OFF) {
        key_up = 1;
    }
    
    return BSP_KEY_NONE;
}