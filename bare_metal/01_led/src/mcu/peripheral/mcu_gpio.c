#include "mcu_gpio.h"
#include "mcu_gpio_reg.h"
#include <stddef.h>

void mcu_gpio_clk_en_port(mcu_gpio_port_t port)
{
    if (!MCU_GPIO_IS_VALID_PORT(port))
    {
        return;
    }
    
    RCC->AHB1ENR |= MCU_GPIO_GET_RCC_EN_BIT(port);
}

void mcu_gpio_en(void)
{
    mcu_gpio_config_t led_config;
    
    /* 1. Enable GPIO port clock for port G (LEDs are on PG13 and PG14) */
    mcu_gpio_clk_en_port(MCU_GPIO_PORT_G);
    
    /* 2. Configure LED0 (PG13) */
    led_config.mode = MCU_GPIO_MODE_OUTPUT;
    led_config.otype = MCU_GPIO_OTYPE_PUSHPULL;
    led_config.ospeed = MCU_GPIO_OSPEED_HIGH;
    led_config.pupd = MCU_GPIO_PUPD_NONE;
    
    mcu_gpio_pin_config(MCU_GPIO_PORT_G, MCU_GPIO_PIN_13, &led_config);
    
    /* 3. Configure LED1 (PG14) */
    mcu_gpio_pin_config(MCU_GPIO_PORT_G, MCU_GPIO_PIN_14, &led_config);
    
    /* 4. Initialize LEDs to off state (active low: HIGH = OFF) */
    mcu_gpio_pin_write(MCU_GPIO_PORT_G, MCU_GPIO_PIN_13, MCU_GPIO_PIN_SET);
    mcu_gpio_pin_write(MCU_GPIO_PORT_G, MCU_GPIO_PIN_14, MCU_GPIO_PIN_SET);
}

void mcu_gpio_pin_config(mcu_gpio_port_t port, mcu_gpio_pin_t pin,
                         const mcu_gpio_config_t* p_config)
{
    GPIO_TypeDef* gpio = MCU_GPIO_GET_PORT_BASE(port);
    
    if (gpio == NULL || p_config == NULL || !MCU_GPIO_IS_VALID_PIN(pin))
    {
        return;
    }
    
    gpio->MODER &= ~MCU_GPIO_MODER_MASK(pin);
    gpio->MODER |= (p_config->mode << MCU_GPIO_MODER_OFFSET(pin));
    
    if (p_config->mode == MCU_GPIO_MODE_OUTPUT || p_config->mode == MCU_GPIO_MODE_ALT)
    {
        gpio->OTYPER &= ~MCU_GPIO_OTYPER_MASK(pin);
        gpio->OTYPER |= (p_config->otype << pin);
    }
    
    if (p_config->mode == MCU_GPIO_MODE_OUTPUT || p_config->mode == MCU_GPIO_MODE_ALT)
    {
        gpio->OSPEEDR &= ~MCU_GPIO_OSPEEDR_MASK(pin);
        gpio->OSPEEDR |= (p_config->ospeed << MCU_GPIO_OSPEEDR_OFFSET(pin));
    }
    
    gpio->PUPDR &= ~MCU_GPIO_PUPDR_MASK(pin);
    gpio->PUPDR |= (p_config->pupd << MCU_GPIO_PUPDR_OFFSET(pin));
}

void mcu_gpio_pin_write(mcu_gpio_port_t port, mcu_gpio_pin_t pin,
                        mcu_gpio_pin_state_t state)
{
    GPIO_TypeDef* gpio = MCU_GPIO_GET_PORT_BASE(port);
    
    if (gpio == NULL || !MCU_GPIO_IS_VALID_PIN(pin))
    {
        return;
    }
    
    if (state == MCU_GPIO_PIN_RESET)
    {
        gpio->BSRR = MCU_GPIO_BSRR_RESET_BIT(pin);
    }
    else
    {
        gpio->BSRR = MCU_GPIO_BSRR_SET_BIT(pin);
    }
}

void mcu_gpio_pin_toggle(mcu_gpio_port_t port, mcu_gpio_pin_t pin)
{
    GPIO_TypeDef* gpio = MCU_GPIO_GET_PORT_BASE(port);
    
    if (gpio == NULL || !MCU_GPIO_IS_VALID_PIN(pin))
    {
        return;
    }
    
    gpio->ODR ^= MCU_GPIO_IDR_MASK(pin);
}

mcu_gpio_pin_state_t mcu_gpio_pin_read(mcu_gpio_port_t port, mcu_gpio_pin_t pin)
{
    GPIO_TypeDef* gpio = MCU_GPIO_GET_PORT_BASE(port);
    
    if (gpio == NULL || !MCU_GPIO_IS_VALID_PIN(pin))
    {
        return MCU_GPIO_PIN_RESET;
    }
    
    if ((gpio->IDR & MCU_GPIO_IDR_MASK(pin)) != 0)
    {
        return MCU_GPIO_PIN_SET;
    }
    else
    {
        return MCU_GPIO_PIN_RESET;
    }
}