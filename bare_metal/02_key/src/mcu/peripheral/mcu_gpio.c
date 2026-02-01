#include "mcu_gpio.h"

void mcu_gpio_config(mcu_gpio_port_t port, mcu_gpio_pin_t pin,
                        const mcu_gpio_config_t* p_config)
{
    RCC->AHB1ENR |= MCU_GPIO_GET_RCC_EN(port);

    GPIO_TypeDef* gpio = MCU_GPIO_GET_PORT(port);
    gpio->MODER &= ~MCU_GPIO_MODER_MASK(pin);
    gpio->MODER |= (p_config->mode << (pin * 2));
    if (p_config->mode == MCU_GPIO_MODE_OUTPUT || p_config->mode == MCU_GPIO_MODE_ALT)
    {
        gpio->OTYPER &= ~MCU_GPIO_OTYPER_MASK(pin);
        gpio->OTYPER |= (p_config->otype << pin);
        gpio->OSPEEDR &= ~MCU_GPIO_OSPEEDR_MASK(pin);
        gpio->OSPEEDR |= (p_config->ospeed << (pin * 2));
    }
    gpio->PUPDR &= ~MCU_GPIO_PUPDR_MASK(pin);
    gpio->PUPDR |= (p_config->pupd << (pin * 2));
    if (p_config->mode == MCU_GPIO_MODE_ALT)
    {
        gpio->AFR[MCU_GPIO_AFR_INDEX(pin)] &= ~MCU_GPIO_AFR_MASK(pin);
        gpio->AFR[MCU_GPIO_AFR_INDEX(pin)] |= (p_config->af << MCU_GPIO_AFR_OFFSET(pin));
    }
}

void mcu_gpio_set(mcu_gpio_port_t port, mcu_gpio_pin_t pin, mcu_gpio_setting_t setting)
{
    GPIO_TypeDef* gpio = MCU_GPIO_GET_PORT(port);

    if (setting == MCU_GPIO_PIN_RESET)
    {
        gpio->BSRR = MCU_GPIO_BSRR_RESET_BIT(pin);
    }
    else
    {
        gpio->BSRR = MCU_GPIO_BSRR_SET_BIT(pin);
    }
}

void mcu_gpio_toggle(mcu_gpio_port_t port, mcu_gpio_pin_t pin)
{
    GPIO_TypeDef* gpio = MCU_GPIO_GET_PORT(port);
    
    if ((gpio->ODR & MCU_GPIO_ODR_MASK(pin)) != 0)
    {
        gpio->BSRR = MCU_GPIO_BSRR_RESET_BIT(pin);
    }
    else
    {
        gpio->BSRR = MCU_GPIO_BSRR_SET_BIT(pin);
    }
}

mcu_gpio_state_t mcu_gpio_get(mcu_gpio_port_t port, mcu_gpio_pin_t pin)
{
    GPIO_TypeDef* gpio = MCU_GPIO_GET_PORT(port);
    
    uint32_t mode = MCU_GPIO_MODER_VALUE(gpio, pin);
    if ( mode == MCU_GPIO_MODE_INPUT )
    {
        return (gpio->IDR & MCU_GPIO_IDR_MASK(pin)) ? MCU_GPIO_HIGH : MCU_GPIO_LOW;
    }
    else if ( mode == MCU_GPIO_MODE_OUTPUT )
    {
        return (gpio->ODR & MCU_GPIO_ODR_MASK(pin)) ? MCU_GPIO_HIGH : MCU_GPIO_LOW;
    }
    else
    {
        return MCU_GPIO_LOW;
    }
}