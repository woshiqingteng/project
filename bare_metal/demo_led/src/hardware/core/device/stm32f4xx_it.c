/**
 * @file stm32f4xx_it.c
 * @brief 中断服务
 */

#include "stm32f429xx.h"

/* 弱定义默认中断处理 */
__attribute__((weak)) void NMI_Handler(void) { while(1); }
__attribute__((weak)) void HardFault_Handler(void) { while(1); }
__attribute__((weak)) void MemManage_Handler(void) { while(1); }
__attribute__((weak)) void BusFault_Handler(void) { while(1); }
__attribute__((weak)) void UsageFault_Handler(void) { while(1); }
__attribute__((weak)) void SVC_Handler(void) { }
__attribute__((weak)) void DebugMon_Handler(void) { }
__attribute__((weak)) void PendSV_Handler(void) { }
__attribute__((weak)) void SysTick_Handler(void) { }