#ifndef MCU_H
#define MCU_H

#include "mcu_system_init.h"
#include "mcu_nvic.h"
#include "mcu_systick.h"
#include "mcu_power.h"
#include "mcu_gpio.h"

void mcu_init_system(void);
void mcu_init_peripheral(void);
void mcu_init(void);

#endif