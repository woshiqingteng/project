/**
 * @file stm32f4xx_it.h
 * @brief 中断头文件
 */

#ifndef STM32F4XX_IT_H
#define STM32F4XX_IT_H

/* 中断处理函数声明 */
void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);

#endif /* STM32F4XX_IT_H */