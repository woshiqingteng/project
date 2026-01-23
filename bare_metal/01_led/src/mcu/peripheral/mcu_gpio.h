#ifndef MCU_GPIO_H
#define MCU_GPIO_H

#include <stdint.h>
#include <stdbool.h>

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
} mcu_gpio_config_t;

/* GPIO enable function */
void mcu_gpio_clk_en_port(mcu_gpio_port_t port);
void mcu_gpio_en(void);

/* Configure GPIO pin */
void mcu_gpio_pin_config(mcu_gpio_port_t port, mcu_gpio_pin_t pin,
                         const mcu_gpio_config_t* p_config);

/* Set GPIO pin state */
void mcu_gpio_pin_write(mcu_gpio_port_t port, mcu_gpio_pin_t pin,
                        mcu_gpio_pin_state_t state);

/* Toggle GPIO pin state */
void mcu_gpio_pin_toggle(mcu_gpio_port_t port, mcu_gpio_pin_t pin);

/* Read GPIO pin state */
mcu_gpio_pin_state_t mcu_gpio_pin_read(mcu_gpio_port_t port, mcu_gpio_pin_t pin);

/* Simple pin write macro */
#define MCU_GPIO_PIN_WRITE(port, pin, state) \
    mcu_gpio_pin_write(port, pin, state)

/* Simple pin toggle macro */
#define MCU_GPIO_PIN_TOGGLE(port, pin) \
    mcu_gpio_pin_toggle(port, pin)

/* Simple pin read macro */
#define MCU_GPIO_PIN_READ(port, pin) \
    mcu_gpio_pin_read(port, pin)

#endif /* MCU_GPIO_H */