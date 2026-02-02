#include "bsp_gpio.h"
#include "bsp_uart.h"
#include "app_uart.h"
#include "sys.h"
#include "delay.h"

// 全局UART句柄
static bsp_uart_handle_t g_uart_handle = NULL;

// 初始化系统
static void system_init(void)
{
    // 系统时钟初始化
    Stm32_Clock_Init(360, 25, 2, 8);  // 180MHz
    
    // 延时初始化
    delay_init(180);
    
    // LED配置
    mcu_gpio_config_t led_config = {
        .mode = MCU_GPIO_MODE_OUTPUT,
        .otype = MCU_GPIO_OTYPE_PP,
        .ospeed = MCU_GPIO_OSPEED_HIGH,
        .pupd = MCU_GPIO_PUPD_NONE,
        .af = MCU_GPIO_AF0
    };
    
    bsp_gpio_config(BSP_GPIO_LED0, &led_config);
    bsp_gpio_set(BSP_GPIO_LED0, MCU_GPIO_PIN_RESET);
    
    // 应用初始化
    app_uart_init();
}

int main(void)
{
    // 系统初始化
    system_init();
    
    // 主循环
    while(1)
    {
        app_uart_run();
    }
}

// UART1中断服务函数
void USART1_IRQHandler(void)
{
    if (g_uart_handle == NULL) {
        // 如果句柄为空，获取句柄
        extern bsp_uart_handle_t bsp_uart_init(void);
        g_uart_handle = bsp_uart_init();
    }
    
    // 处理中断
    extern void bsp_uart_irq_handler(bsp_uart_handle_t handle);
    bsp_uart_irq_handler(g_uart_handle);
}