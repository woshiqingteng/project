#include "mcu_gpio.h"
#include "mcu_gpio_reg.h"
#include <stdbool.h>
#include <stddef.h>

void mcu_gpio_init(mcu_gpio_port_t port)
{
    RCC->AHB1ENR |= MCU_GPIO_GET_RCC_EN(port);
}

void mcu_gpio_pin_config(mcu_gpio_port_t port, mcu_gpio_pin_t pin,
                         const mcu_gpio_config_t* p_config)
{
    GPIO_TypeDef* gpio = MCU_GPIO_GET_PORT(port);
    
    /* Configure MODE register */
    gpio->MODER &= ~MCU_GPIO_MODER_MASK(pin);
    gpio->MODER |= (p_config->mode << (pin * 2));
    
    /* Configure OTYPER register (only for output or alternate mode) */
    if (p_config->mode == MCU_GPIO_MODE_OUTPUT || p_config->mode == MCU_GPIO_MODE_ALT)
    {
        gpio->OTYPER &= ~MCU_GPIO_OTYPER_MASK(pin);
        gpio->OTYPER |= (p_config->otype << pin);
    }
    
    /* Configure OSPEEDR register (only for output or alternate mode) */
    if (p_config->mode == MCU_GPIO_MODE_OUTPUT || p_config->mode == MCU_GPIO_MODE_ALT)
    {
        gpio->OSPEEDR &= ~MCU_GPIO_OSPEEDR_MASK(pin);
        gpio->OSPEEDR |= (p_config->ospeed << (pin * 2));
    }
    
    /* Configure PUPDR register */
    gpio->PUPDR &= ~MCU_GPIO_PUPDR_MASK(pin);
    gpio->PUPDR |= (p_config->pupd << (pin * 2));
    
    /* Configure alternate function if mode is ALT */
    if (p_config->mode == MCU_GPIO_MODE_ALT)
    {
        mcu_gpio_pin_af_config(port, pin, p_config->af);
    }
}

void mcu_gpio_pin_af_config(mcu_gpio_port_t port, mcu_gpio_pin_t pin,
                           mcu_gpio_af_t af)
{
    GPIO_TypeDef* gpio = MCU_GPIO_GET_PORT(port);
    
    /* Clear AF selection bits for the pin */
    gpio->AFR[MCU_GPIO_AFR_INDEX(pin)] &= ~MCU_GPIO_AFR_MASK(pin);
    
    /* Set new AF selection */
    gpio->AFR[MCU_GPIO_AFR_INDEX(pin)] |= (af << MCU_GPIO_AFR_OFFSET(pin));
}

void mcu_gpio_pin_write(mcu_gpio_port_t port, mcu_gpio_pin_t pin,
                        mcu_gpio_pin_state_t state)
{
    GPIO_TypeDef* gpio = MCU_GPIO_GET_PORT(port);
    
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
    GPIO_TypeDef* gpio = MCU_GPIO_GET_PORT(port);
    
    gpio->ODR ^= MCU_GPIO_IDR_MASK(pin);
}

mcu_gpio_pin_state_t mcu_gpio_pin_read(mcu_gpio_port_t port, mcu_gpio_pin_t pin)
{
    GPIO_TypeDef* gpio = MCU_GPIO_GET_PORT(port);
    
    if ((gpio->IDR & MCU_GPIO_IDR_MASK(pin)) != 0)
    {
        return MCU_GPIO_PIN_SET;
    }
    else
    {
        return MCU_GPIO_PIN_RESET;
    }
}