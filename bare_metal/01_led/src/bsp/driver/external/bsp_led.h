#ifndef BSP_LED_H
#define BSP_LED_H

#include <stdbool.h>

/* LED枚举 */
typedef enum {
    BSP_LED_0 = 0,
    BSP_LED_1,
    BSP_LED_COUNT
} bsp_led_id_t;

/* LED状态 */
typedef enum {
    BSP_LED_OFF = 0,
    BSP_LED_ON
} bsp_led_state_t;

/* LED引脚映射宏 */
#define BSP_LED_GET_PIN(led_id) \
    ((led_id) == BSP_LED_0 ? BSP_GPIO_PIN_LED0 : \
     (led_id) == BSP_LED_1 ? BSP_GPIO_PIN_LED1 : \
     BSP_GPIO_PIN_LED0)   /* 默认返回LED0引脚 */

/* LED初始化 */
bool bsp_led_init(void);

/* LED控制 */
bool bsp_led_set(bsp_led_id_t led_id, bsp_led_state_t state);
bool bsp_led_toggle(bsp_led_id_t led_id);
bool bsp_led_get(bsp_led_id_t led_id, bsp_led_state_t *state);

#endif /* BSP_LED_H */