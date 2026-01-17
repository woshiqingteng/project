/**
 * @file bsp_rcc.h
 * @brief 时钟配置头文件
 */

#ifndef BSP_RCC_H
#define BSP_RCC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * @brief 系统时钟初始化
 * @note 配置HSE 25MHz -> PLL 180MHz系统时钟
 */
void bsp_rcc_init(void);

/**
 * @brief 毫秒延时
 * @param ms 毫秒数
 */
void bsp_delay_ms(uint32_t ms);

#ifdef __cplusplus
}
#endif

#endif /* BSP_RCC_H */