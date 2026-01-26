#ifndef MCU_SYSTICK_H
#define MCU_SYSTICK_H

#include <stdint.h>

void mcu_systick_config(uint32_t sysclk);
void mcu_systick_delay_us(uint32_t us);
void mcu_systick_delay_ms(uint16_t ms);

#endif