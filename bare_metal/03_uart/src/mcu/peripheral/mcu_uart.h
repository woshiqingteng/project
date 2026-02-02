#ifndef MCU_UART_H
#define MCU_UART_H

#include "stm32f4xx.h"
#include <stdint.h>
#include <stdbool.h>

// UART寄存器操作结构体
typedef struct {
    USART_TypeDef* instance;
} mcu_uart_t;

// UART配置结构体
typedef struct {
    uint32_t baudrate;
    uint8_t data_bits;
    uint8_t stop_bits;
    uint8_t parity;
    bool enable_tx;
    bool enable_rx;
    bool enable_irq;
} mcu_uart_config_t;

// 函数声明
void mcu_uart_init(mcu_uart_t* uart, const mcu_uart_config_t* config);
void mcu_uart_send_byte(mcu_uart_t* uart, uint8_t data);
void mcu_uart_send_buffer(mcu_uart_t* uart, const uint8_t* buffer, uint16_t size);
uint8_t mcu_uart_receive_byte(mcu_uart_t* uart);
void mcu_uart_enable_irq(mcu_uart_t* uart, bool enable);
void mcu_uart_disable_irq(mcu_uart_t* uart);

#endif