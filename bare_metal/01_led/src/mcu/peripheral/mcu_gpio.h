#ifndef MCU_GPIO_H
#define MCU_GPIO_H

#include <stdint.h>

/* GPIO port identifiers */
typedef enum {
    MCU_GPIO_PORT_A = 0,
    MCU_GPIO_PORT_B,
    MCU_GPIO_PORT_C,
    MCU_GPIO_PORT_D,
    MCU_GPIO_PORT_E,
    MCU_GPIO_PORT_F,
    MCU_GPIO_PORT_G,
    MCU_GPIO_PORT_H,
    MCU_GPIO_PORT_I
} mcu_gpio_port_t;

/* GPIO pin numbers */
typedef enum {
    MCU_GPIO_PIN_0 = 0,
    MCU_GPIO_PIN_1,
    MCU_GPIO_PIN_2,
    MCU_GPIO_PIN_3,
    MCU_GPIO_PIN_4,
    MCU_GPIO_PIN_5,
    MCU_GPIO_PIN_6,
    MCU_GPIO_PIN_7,
    MCU_GPIO_PIN_8,
    MCU_GPIO_PIN_9,
    MCU_GPIO_PIN_10,
    MCU_GPIO_PIN_11,
    MCU_GPIO_PIN_12,
    MCU_GPIO_PIN_13,
    MCU_GPIO_PIN_14,
    MCU_GPIO_PIN_15
} mcu_gpio_pin_t;

/* GPIO alternate function selection */
typedef enum {
    MCU_GPIO_AF0 = 0,
    MCU_GPIO_AF1,
    MCU_GPIO_AF2,
    MCU_GPIO_AF3,
    MCU_GPIO_AF4,
    MCU_GPIO_AF5,
    MCU_GPIO_AF6,
    MCU_GPIO_AF7,
    MCU_GPIO_AF8,
    MCU_GPIO_AF9,
    MCU_GPIO_AF10,
    MCU_GPIO_AF11,
    MCU_GPIO_AF12,
    MCU_GPIO_AF13,
    MCU_GPIO_AF14,
    MCU_GPIO_AF15
} mcu_gpio_af_t;

/* GPIO mode configuration */
typedef enum {
    MCU_GPIO_MODE_INPUT = 0,
    MCU_GPIO_MODE_OUTPUT,
    MCU_GPIO_MODE_ALT,
    MCU_GPIO_MODE_ANALOG
} mcu_gpio_mode_t;

/* GPIO output type */
typedef enum {
    MCU_GPIO_OTYPE_PUSHPULL = 0,
    MCU_GPIO_OTYPE_OPENDRAIN
} mcu_gpio_otype_t;

/* GPIO output speed */
typedef enum {
    MCU_GPIO_OSPEED_LOW = 0,
    MCU_GPIO_OSPEED_MEDIUM,
    MCU_GPIO_OSPEED_FAST,
    MCU_GPIO_OSPEED_HIGH
} mcu_gpio_ospeed_t;

/* GPIO pull-up/pull-down configuration */
typedef enum {
    MCU_GPIO_PUPD_NONE = 0,
    MCU_GPIO_PUPD_UP,
    MCU_GPIO_PUPD_DOWN
} mcu_gpio_pupd_t;

/* GPIO pin state */
typedef enum {
    MCU_GPIO_PIN_RESET = 0,
    MCU_GPIO_PIN_SET
} mcu_gpio_pin_state_t;

/* GPIO configuration structure */
typedef struct
{
    mcu_gpio_mode_t mode;
    mcu_gpio_otype_t otype;
    mcu_gpio_ospeed_t ospeed;
    mcu_gpio_pupd_t pupd;
    mcu_gpio_af_t af;
} mcu_gpio_config_t;

/* GPIO initialization */
void mcu_gpio_init(mcu_gpio_port_t port);

/* GPIO pin configuration */
void mcu_gpio_pin_config(mcu_gpio_port_t port, mcu_gpio_pin_t pin,
                         const mcu_gpio_config_t* p_config);

/* GPIO pin alternate function configuration */
void mcu_gpio_pin_af_config(mcu_gpio_port_t port, mcu_gpio_pin_t pin,
                           mcu_gpio_af_t af);

/* GPIO pin control */
void mcu_gpio_pin_write(mcu_gpio_port_t port, mcu_gpio_pin_t pin,
                        mcu_gpio_pin_state_t state);
void mcu_gpio_pin_toggle(mcu_gpio_port_t port, mcu_gpio_pin_t pin);
mcu_gpio_pin_state_t mcu_gpio_pin_read(mcu_gpio_port_t port, mcu_gpio_pin_t pin);

#endif