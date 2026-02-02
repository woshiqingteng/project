#ifndef MCU_POWER_H
#define MCU_POWER_H

void mcu_power_enter_standby(void);
void mcu_power_wait_for_interrupt(void);
void mcu_power_disable_interrupt(void);
void mcu_power_enable_interrupt(void);

#endif