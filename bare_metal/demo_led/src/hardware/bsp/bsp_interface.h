/**
 * @file bsp_interface.h
 * @brief 板级层接口
 */

#ifndef BSP_INTERFACE_H
#define BSP_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* LED定义 */
#define LED0 0
#define LED1 1

/**
 * @brief 板级初始化
 */
void bsp_init(void);

/**
 * @brief 设置LED状态
 * @param led_id LED编号
 * @param state 状态 (1=亮, 0=灭)
 */
void bsp_led_set(uint8_t led_id, uint8_t state);

/**
 * @brief 切换LED状态
 * @param led_id LED编号
 */
void bsp_led_toggle(uint8_t led_id);

/**
 * @brief 毫秒延时
 * @param ms 毫秒数
 */
void bsp_delay_ms(uint32_t ms);

#ifdef __cplusplus
}
#endif

#endif /* BSP_INTERFACE_H */