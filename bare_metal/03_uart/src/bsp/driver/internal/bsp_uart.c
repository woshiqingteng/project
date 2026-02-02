#include "bsp_uart.h"
#include "bsp_gpio.h"
#include "mcu_uart.h"

// 私有结构体定义
struct bsp_uart_t {
    mcu_uart_t mcu_uart;
    bsp_uart_rx_callback_t rx_callback;
    uint8_t rx_buffer[256];
    uint16_t rx_length;
    bool rx_complete;
    uint16_t counter;
    bool led_state;
};

// 全局实例
static struct bsp_uart_t s_uart_instance = {0};

// 私有函数声明
static void bsp_uart_send_byte_internal(bsp_uart_handle_t handle, uint8_t data);
static void bsp_uart_send_string_internal(bsp_uart_handle_t handle, const char* str);
static void bsp_uart_send_buffer_internal(bsp_uart_handle_t handle, const uint8_t* buffer, uint16_t size);
static void bsp_uart_set_rx_callback_internal(bsp_uart_handle_t handle, bsp_uart_rx_callback_t callback);

// 函数指针表
static const bsp_uart_ops_t s_uart_ops = {
    .send_byte = bsp_uart_send_byte_internal,
    .send_string = bsp_uart_send_string_internal,
    .send_buffer = bsp_uart_send_buffer_internal,
    .set_rx_callback = bsp_uart_set_rx_callback_internal
};

// 初始化GPIO
static void init_gpio(void)
{
    // 配置PA9为TX，PA10为RX
    mcu_gpio_config_t gpio_config = {
        .mode = MCU_GPIO_MODE_ALT,
        .otype = MCU_GPIO_OTYPE_PP,
        .ospeed = MCU_GPIO_OSPEED_HIGH,
        .pupd = MCU_GPIO_PUPD_UP,
        .af = MCU_GPIO_AF7
    };
    
    // 配置TX引脚
    bsp_gpio_config(BSP_GPIO_LED0, &gpio_config); // 复用LED0配置
    // 配置RX引脚
    bsp_gpio_config(BSP_GPIO_LED1, &gpio_config); // 复用LED1配置
}

// 初始化UART
bsp_uart_handle_t bsp_uart_init(void)
{
    // 初始化GPIO
    init_gpio();
    
    // 设置UART实例
    s_uart_instance.mcu_uart.instance = USART1;
    
    // UART配置
    mcu_uart_config_t uart_config = {
        .baudrate = 115200,
        .data_bits = 8,
        .stop_bits = 1,
        .parity = 0,
        .enable_tx = true,
        .enable_rx = true,
        .enable_irq = true
    };
    
    // 初始化UART
    mcu_uart_init(&s_uart_instance.mcu_uart, &uart_config);
    
    // 初始化状态
    s_uart_instance.rx_callback = NULL;
    s_uart_instance.rx_length = 0;
    s_uart_instance.rx_complete = false;
    s_uart_instance.counter = 0;
    s_uart_instance.led_state = false;
    
    return &s_uart_instance;
}

// 获取函数指针表
const bsp_uart_ops_t* bsp_uart_get_ops(void)
{
    return &s_uart_ops;
}

// 内部发送字节函数
static void bsp_uart_send_byte_internal(bsp_uart_handle_t handle, uint8_t data)
{
    struct bsp_uart_t* uart = (struct bsp_uart_t*)handle;
    mcu_uart_send_byte(&uart->mcu_uart, data);
}

// 内部发送字符串函数
static void bsp_uart_send_string_internal(bsp_uart_handle_t handle, const char* str)
{
    if (!str) return;
    
    while (*str) {
        bsp_uart_send_byte_internal(handle, *str++);
    }
}

// 内部发送缓冲区函数
static void bsp_uart_send_buffer_internal(bsp_uart_handle_t handle, const uint8_t* buffer, uint16_t size)
{
    if (!buffer || size == 0) return;
    
    struct bsp_uart_t* uart = (struct bsp_uart_t*)handle;
    mcu_uart_send_buffer(&uart->mcu_uart, buffer, size);
}

// 内部设置回调函数
static void bsp_uart_set_rx_callback_internal(bsp_uart_handle_t handle, bsp_uart_rx_callback_t callback)
{
    struct bsp_uart_t* uart = (struct bsp_uart_t*)handle;
    uart->rx_callback = callback;
}

// 中断处理函数（需要在外部调用）
void bsp_uart_irq_handler(bsp_uart_handle_t handle)
{
    struct bsp_uart_t* uart = (struct bsp_uart_t*)handle;
    
    if (uart->mcu_uart.instance->SR & USART_SR_RXNE) {
        uint8_t data = (uint8_t)(uart->mcu_uart.instance->DR & 0xFF);
        
        // 保存到缓冲区
        if (uart->rx_length < 255) {
            uart->rx_buffer[uart->rx_length++] = data;
            
            // 检测换行符
            if (data == '\n') {
                uart->rx_buffer[uart->rx_length] = '\0';
                uart->rx_complete = true;
            }
        }
        
        // 调用回调
        if (uart->rx_callback) {
            uart->rx_callback(data);
        }
    }
}

// 获取接收状态（内部使用）
static bool get_rx_complete(bsp_uart_handle_t handle)
{
    struct bsp_uart_t* uart = (struct bsp_uart_t*)handle;
    return uart->rx_complete;
}

// 获取接收缓冲区（内部使用）
static uint8_t* get_rx_buffer(bsp_uart_handle_t handle)
{
    struct bsp_uart_t* uart = (struct bsp_uart_t*)handle;
    return uart->rx_buffer;
}

// 获取接收长度（内部使用）
static uint16_t get_rx_length(bsp_uart_handle_t handle)
{
    struct bsp_uart_t* uart = (struct bsp_uart_t*)handle;
    return uart->rx_length;
}

// 清除接收状态（内部使用）
static void clear_rx_state(bsp_uart_handle_t handle)
{
    struct bsp_uart_t* uart = (struct bsp_uart_t*)handle;
    uart->rx_length = 0;
    uart->rx_complete = false;
}

// 获取计数器（内部使用）
static uint16_t get_counter(bsp_uart_handle_t handle)
{
    struct bsp_uart_t* uart = (struct bsp_uart_t*)handle;
    return uart->counter;
}

// 增加计数器（内部使用）
static void increment_counter(bsp_uart_handle_t handle)
{
    struct bsp_uart_t* uart = (struct bsp_uart_t*)handle;
    uart->counter++;
}

// 获取LED状态（内部使用）
static bool get_led_state(bsp_uart_handle_t handle)
{
    struct bsp_uart_t* uart = (struct bsp_uart_t*)handle;
    return uart->led_state;
}

// 切换LED状态（内部使用）
static void toggle_led_state(bsp_uart_handle_t handle)
{
    struct bsp_uart_t* uart = (struct bsp_uart_t*)handle;
    uart->led_state = !uart->led_state;
}