#ifndef MCU_INIT_H
#define MCU_INIT_H

#include <stdint.h>
#include <stdbool.h>
#include "mcu_system_init.h"
#include "mcu_nvic.h"
#include "mcu_systick.h"
#include "mcu_power.h"
#include "mcu_gpio.h"

void mcu_init_system(void);
void mcu_init_peripheral(void);
void mcu_init(void);

#endif