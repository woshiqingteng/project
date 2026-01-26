#ifndef BSP_GPIO_H
#define BSP_GPIO_H

#include "mcu_gpio.h"
#include "bsp_gpio_pin.h"
#include <stdbool.h>

typedef enum {
    BSP_GPIO_PIN_LED0,
    BSP_GPIO_PIN_LED1,
    BSP_GPIO_PIN_COUNT
} bsp_gpio_pin_t;

typedef mcu_gpio_pin_state_t bsp_gpio_state_t;

/* 
 * BSP GPIO配置结构体
 * 注意：去除了af成员，因为BSP层不需要关心复用功能
 * 复用功能配置在需要时通过专门的API完成
 */
typedef struct {
    mcu_gpio_mode_t mode;
    mcu_gpio_otype_t otype;
    mcu_gpio_ospeed_t ospeed;
    mcu_gpio_pupd_t pupd;
} bsp_gpio_config_t;

/* GPIO引脚配置 */
bool bsp_gpio_pin_config(bsp_gpio_pin_t pin, const bsp_gpio_config_t *config);

/* GPIO引脚复用功能配置 */
bool bsp_gpio_pin_af_config(bsp_gpio_pin_t pin, mcu_gpio_af_t af);

/* GPIO引脚控制 */
void bsp_gpio_pin_set(bsp_gpio_pin_t pin, bsp_gpio_state_t state);
bsp_gpio_state_t bsp_gpio_pin_get(bsp_gpio_pin_t pin);
void bsp_gpio_pin_toggle(bsp_gpio_pin_t pin);

#endif /* BSP_GPIO_H */