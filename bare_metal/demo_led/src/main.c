/**
 * @file main.c
 * @brief 应用主入口
 */

#include "app/app_interface.h"

/**
 * @brief 主函数
 */
int main(void)
{
    /* 应用初始化 */
    app_init();
    
    /* 主循环 */
    while (1)
    {
        app_run();
    }
    
    return 0;
}