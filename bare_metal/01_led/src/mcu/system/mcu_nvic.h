#ifndef MCU_NVIC_H
#define MCU_NVIC_H

#include <stdint.h>
#include "stm32f4xx.h"

void mcu_nvic_prio_group_cfg(uint8_t nvic_group);
void mcu_nvic_init(uint8_t preemption_priority, uint8_t sub_priority, 
               uint8_t channel, uint8_t group);
void mcu_nvic_exti_config(uint8_t gpio_x, uint8_t bit_x, uint8_t trim);

#endif