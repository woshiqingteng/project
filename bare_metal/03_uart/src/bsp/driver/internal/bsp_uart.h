#ifndef BSP_UART_H
#define BSP_UART_H

#include <stdint.h>
#include <stdbool.h>

// 回调函数类型
typedef void (*bsp_uart_rx_callback_t)(uint8_t data);

// 不透明指针类型
typedef struct bsp_uart_t* bsp_uart_handle_t;

// 函数指针表类型
typedef struct {
    void (*send_byte)(bsp_uart_handle_t, uint8_t);
    void (*send_string)(bsp_uart_handle_t, const char*);
    void (*send_buffer)(bsp_uart_handle_t, const uint8_t*, uint16_t);
    void (*set_rx_callback)(bsp_uart_handle_t, bsp_uart_rx_callback_t);
} bsp_uart_ops_t;

// API函数声明
bsp_uart_handle_t bsp_uart_init(void);
const bsp_uart_ops_t* bsp_uart_get_ops(void);

#endif