#ifndef MCU_SYSTEM_INIT_H
#define MCU_SYSTEM_INIT_H

#include <stdint.h>

typedef struct
{
    uint32_t pll_n;
    uint32_t pll_m;
    uint32_t pll_p;
    uint32_t pll_q;
} mcu_system_clock_config_t;

uint8_t mcu_system_clock_init(const mcu_system_clock_config_t* p_config);
void mcu_system_init(uint32_t pll_n, uint32_t pll_m, uint32_t pll_p, uint32_t pll_q);
void mcu_system_set_vector_table(uint32_t nvic_vect_tab, uint32_t offset);
void mcu_system_soft_reset(void);
void mcu_system_set_stack_pointer(uint32_t addr);

#endif