/**
 * @file app_led.h
 * @brief LED应用定义
 */

#ifndef APP_LED_H
#define APP_LED_H

#include <stdint.h>

/* LED模式 */
#define LED_MODE_OFF         0
#define LED_MODE_ON          1
#define LED_MODE_SLOW_BLINK  2
#define LED_MODE_FAST_BLINK  3
#define LED_MODE_ALTERNATE   4
#define LED_MODE_MAX         5

/**
 * @brief 设置LED模式
 * @param mode 模式
 */
void app_led_set_mode(uint8_t mode);

#endif /* APP_LED_H */