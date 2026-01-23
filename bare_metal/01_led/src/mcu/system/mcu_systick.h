#ifndef MCU_SYSTICK_H
#define MCU_SYSTICK_H

#include <stdint.h>
#include "stm32f4xx.h"

uint32_t mcu_systick_init(uint32_t ticks);
void mcu_systick_config(uint32_t sysclk);
void mcu_systick_delay_us(uint32_t us);
void mcu_systick_delay_ms(uint16_t ms);
uint32_t mcu_systick_get_counter(void);

#endif