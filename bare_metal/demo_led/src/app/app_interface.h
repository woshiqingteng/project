/**
 * @file app_interface.h
 * @brief 应用层接口
 */

#ifndef APP_INTERFACE_H
#define APP_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 应用初始化
 */
void app_init(void);

/**
 * @brief 应用主循环
 */
void app_run(void);

#ifdef __cplusplus
}
#endif

#endif /* APP_INTERFACE_H */