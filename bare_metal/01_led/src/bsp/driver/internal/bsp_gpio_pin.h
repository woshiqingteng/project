#ifndef BSP_GPIO_PIN_H
#define BSP_GPIO_PIN_H

#include "mcu_gpio.h"

// LED Pin Definitions
#define BSP_LED0_PORT      MCU_GPIO_PORT_G
#define BSP_LED0_PIN       MCU_GPIO_PIN_13

#define BSP_LED1_PORT      MCU_GPIO_PORT_G
#define BSP_LED1_PIN       MCU_GPIO_PIN_14

// GPIO Pin Mapping Macros
#define BSP_GPIO_GET_MCU_PORT(pin) \
    ((pin) == BSP_GPIO_PIN_LED0 ? BSP_LED0_PORT : \
     (pin) == BSP_GPIO_PIN_LED1 ? BSP_LED1_PORT : MCU_GPIO_PORT_A)

#define BSP_GPIO_GET_MCU_PIN(pin) \
    ((pin) == BSP_GPIO_PIN_LED0 ? BSP_LED0_PIN : \
     (pin) == BSP_GPIO_PIN_LED1 ? BSP_LED1_PIN : MCU_GPIO_PIN_0)

#endif /* BSP_GPIO_PIN_H */