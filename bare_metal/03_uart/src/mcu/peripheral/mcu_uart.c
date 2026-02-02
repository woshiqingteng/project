#include "mcu_uart.h"

// 计算波特率分频值（直接计算）
static uint32_t calculate_brr(uint32_t baudrate, uint32_t pclk)
{
    return pclk / baudrate;
}

// 获取PCLK频率
static uint32_t get_pclk_frequency(USART_TypeDef* instance)
{
    if (instance == USART1 || instance == USART6) {
        return 90000000; // APB2时钟，90MHz
    } else {
        return 45000000; // APB1时钟，45MHz
    }
}

// 获取RCC使能寄存器
static uint32_t get_rcc_enable(USART_TypeDef* instance)
{
    if (instance == USART1) {
        return RCC_APB2ENR_USART1EN;
    } else if (instance == USART2) {
        return RCC_APB1ENR_USART2EN;
    } else if (instance == USART3) {
        return RCC_APB1ENR_USART3EN;
    } else if (instance == USART6) {
        return RCC_APB2ENR_USART6EN;
    }
    return 0;
}

// 获取IRQ编号
static IRQn_Type get_irqn(USART_TypeDef* instance)
{
    if (instance == USART1) {
        return USART1_IRQn;
    } else if (instance == USART2) {
        return USART2_IRQn;
    } else if (instance == USART3) {
        return USART3_IRQn;
    } else if (instance == USART6) {
        return USART6_IRQn;
    }
    return USART1_IRQn;
}

// 初始化UART
void mcu_uart_init(mcu_uart_t* uart, const mcu_uart_config_t* config)
{
    if (!uart || !uart->instance || !config) return;
    
    // 使能时钟
    if (uart->instance == USART1 || uart->instance == USART6) {
        RCC->APB2ENR |= get_rcc_enable(uart->instance);
    } else {
        RCC->APB1ENR |= get_rcc_enable(uart->instance);
    }
    
    // 禁用UART
    uart->instance->CR1 &= ~USART_CR1_UE;
    
    // 计算波特率
    uint32_t pclk = get_pclk_frequency(uart->instance);
    uint32_t brr = calculate_brr(config->baudrate, pclk);
    uart->instance->BRR = brr;
    
    // 配置控制寄存器
    uart->instance->CR1 = 0;
    
    // 数据位
    if (config->data_bits == 9) {
        uart->instance->CR1 |= USART_CR1_M;
    }
    
    // 校验位
    if (config->parity != 0) {
        uart->instance->CR1 |= USART_CR1_PCE;
        if (config->parity == 2) {
            uart->instance->CR1 |= USART_CR1_PS; // 奇校验
        }
    }
    
    // 停止位
    if (config->stop_bits == 2) {
        uart->instance->CR2 |= USART_CR2_STOP_1;
    }
    
    // 使能发送/接收
    if (config->enable_tx) {
        uart->instance->CR1 |= USART_CR1_TE;
    }
    if (config->enable_rx) {
        uart->instance->CR1 |= USART_CR1_RE;
    }
    
    // 使能UART
    uart->instance->CR1 |= USART_CR1_UE;
    
    // 使能中断
    if (config->enable_irq) {
        mcu_uart_enable_irq(uart, true);
    }
}

// 发送一个字节
void mcu_uart_send_byte(mcu_uart_t* uart, uint8_t data)
{
    if (!uart || !uart->instance) return;
    
    // 等待发送缓冲区空
    while (!(uart->instance->SR & USART_SR_TXE));
    
    // 发送数据
    uart->instance->DR = data;
}

// 发送缓冲区数据
void mcu_uart_send_buffer(mcu_uart_t* uart, const uint8_t* buffer, uint16_t size)
{
    if (!uart || !uart->instance || !buffer) return;
    
    for (uint16_t i = 0; i < size; i++) {
        mcu_uart_send_byte(uart, buffer[i]);
    }
}

// 接收一个字节
uint8_t mcu_uart_receive_byte(mcu_uart_t* uart)
{
    if (!uart || !uart->instance) return 0;
    
    // 等待接收数据
    while (!(uart->instance->SR & USART_SR_RXNE));
    
    return (uint8_t)(uart->instance->DR & 0xFF);
}

// 使能中断
void mcu_uart_enable_irq(mcu_uart_t* uart, bool enable)
{
    if (!uart || !uart->instance) return;
    
    if (enable) {
        uart->instance->CR1 |= USART_CR1_RXNEIE;
        
        // 配置NVIC
        IRQn_Type irqn = get_irqn(uart->instance);
        NVIC_EnableIRQ(irqn);
        NVIC_SetPriority(irqn, 3);
    } else {
        uart->instance->CR1 &= ~USART_CR1_RXNEIE;
    }
}

// 禁用中断
void mcu_uart_disable_irq(mcu_uart_t* uart)
{
    mcu_uart_enable_irq(uart, false);
}