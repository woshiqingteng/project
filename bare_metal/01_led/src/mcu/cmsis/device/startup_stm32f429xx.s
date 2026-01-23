/**
 * @file startup_stm32f429xx.s
 * @brief STM32F429xx Devices vector table for GCC based toolchains
 * @author QingTeng
 * @version 1.0.0
 * @date 2025-01-18
 * 
 */

.syntax unified
.cpu cortex-m4
.fpu fpv4-sp-d16
.thumb

.extern _estack, _sidata, _sdata, _edata, _sbss, _ebss
.extern SystemInit, main

.global g_pfnVectors
.global Default_Handler

.section .isr_vector,"a",%progbits
.type g_pfnVectors, %object

g_pfnVectors:
    .word  _estack
    .word  Reset_Handler

    .word  NMI_Handler
    .word  HardFault_Handler
    .word  MemManage_Handler
    .word  BusFault_Handler
    .word  UsageFault_Handler
    .word  0
    .word  0
    .word  0
    .word  0
    .word  SVC_Handler
    .word  DebugMon_Handler
    .word  0
    .word  PendSV_Handler
    .word  SysTick_Handler

    .word     WWDG_IRQHandler
    .word     PVD_IRQHandler
    .word     TAMP_STAMP_IRQHandler
    .word     RTC_WKUP_IRQHandler
    .word     FLASH_IRQHandler
    .word     RCC_IRQHandler
    .word     EXTI0_IRQHandler
    .word     EXTI1_IRQHandler
    .word     EXTI2_IRQHandler
    .word     EXTI3_IRQHandler
    .word     EXTI4_IRQHandler
    .word     DMA1_Stream0_IRQHandler
    .word     DMA1_Stream1_IRQHandler
    .word     DMA1_Stream2_IRQHandler
    .word     DMA1_Stream3_IRQHandler
    .word     DMA1_Stream4_IRQHandler
    .word     DMA1_Stream5_IRQHandler
    .word     DMA1_Stream6_IRQHandler
    .word     ADC_IRQHandler
    .word     CAN1_TX_IRQHandler
    .word     CAN1_RX0_IRQHandler
    .word     CAN1_RX1_IRQHandler
    .word     CAN1_SCE_IRQHandler
    .word     EXTI9_5_IRQHandler
    .word     TIM1_BRK_TIM9_IRQHandler
    .word     TIM1_UP_TIM10_IRQHandler
    .word     TIM1_TRG_COM_TIM11_IRQHandler
    .word     TIM1_CC_IRQHandler
    .word     TIM2_IRQHandler
    .word     TIM3_IRQHandler
    .word     TIM4_IRQHandler
    .word     I2C1_EV_IRQHandler
    .word     I2C1_ER_IRQHandler
    .word     I2C2_EV_IRQHandler
    .word     I2C2_ER_IRQHandler
    .word     SPI1_IRQHandler
    .word     SPI2_IRQHandler
    .word     USART1_IRQHandler
    .word     USART2_IRQHandler
    .word     USART3_IRQHandler
    .word     EXTI15_10_IRQHandler
    .word     RTC_Alarm_IRQHandler
    .word     OTG_FS_WKUP_IRQHandler
    .word     TIM8_BRK_TIM12_IRQHandler
    .word     TIM8_UP_TIM13_IRQHandler 
    .word     TIM8_TRG_COM_TIM14_IRQHandler  
    .word     TIM8_CC_IRQHandler     
    .word     DMA1_Stream7_IRQHandler
    .word     FMC_IRQHandler
    .word     SDIO_IRQHandler
    .word     TIM5_IRQHandler
    .word     SPI3_IRQHandler
    .word     UART4_IRQHandler
    .word     UART5_IRQHandler
    .word     TIM6_DAC_IRQHandler
    .word     TIM7_IRQHandler
    .word     DMA2_Stream0_IRQHandler
    .word     DMA2_Stream1_IRQHandler
    .word     DMA2_Stream2_IRQHandler
    .word     DMA2_Stream3_IRQHandler
    .word     DMA2_Stream4_IRQHandler
    .word     ETH_IRQHandler
    .word     ETH_WKUP_IRQHandler
    .word     CAN2_TX_IRQHandler
    .word     CAN2_RX0_IRQHandler
    .word     CAN2_RX1_IRQHandler
    .word     CAN2_SCE_IRQHandler
    .word     OTG_FS_IRQHandler
    .word     DMA2_Stream5_IRQHandler
    .word     DMA2_Stream6_IRQHandler
    .word     DMA2_Stream7_IRQHandler
    .word     USART6_IRQHandler
    .word     I2C3_EV_IRQHandler
    .word     I2C3_ER_IRQHandler
    .word     OTG_HS_EP1_OUT_IRQHandler
    .word     OTG_HS_EP1_IN_IRQHandler
    .word     OTG_HS_WKUP_IRQHandler
    .word     OTG_HS_IRQHandler
    .word     DCMI_IRQHandler
    .word     0
    .word     HASH_RNG_IRQHandler
    .word     FPU_IRQHandler
    .word     UART7_IRQHandler
    .word     UART8_IRQHandler
    .word     SPI4_IRQHandler
    .word     SPI5_IRQHandler
    .word     SPI6_IRQHandler
    .word     SAI1_IRQHandler
    .word     LTDC_IRQHandler
    .word     LTDC_ER_IRQHandler
    .word     DMA2D_IRQHandler

    .size g_pfnVectors, .-g_pfnVectors

.section .text.Default_Handler,"ax",%progbits
Default_Handler:
Infinite_Loop:
    b  Infinite_Loop
    .size  Default_Handler, .-Default_Handler

.section .text.Reset_Handler
.weak Reset_Handler
.type Reset_Handler, %function

Reset_Handler:
    /* Enable FPU */
    ldr     r0, =0xE000ED88
    ldr     r1, [r0]
    orr     r1, r1, #(0xF << 20)
    str     r1, [r0]
    dsb
    isb

    ldr sp, =_estack
    ldr r0, =_sdata
    ldr r1, =_edata
    ldr r2, =_sidata
    movs r3, #0
    b LoopCopyDataInit

CopyDataInit:
    ldr r4, [r2, r3]
    str r4, [r0, r3]
    adds r3, r3, #4

LoopCopyDataInit:
    adds r4, r0, r3
    cmp r4, r1
    bcc CopyDataInit
    ldr r2, =_sbss
    ldr r4, =_ebss
    movs r3, #0
    b LoopFillZerobss

FillZerobss:
    str  r3, [r2]
    adds r2, r2, #4

LoopFillZerobss:
    cmp r2, r4
    bcc FillZerobss
    bl  SystemInit   
    bl  main
    bx  lr

    .size  Reset_Handler, .-Reset_Handler

.macro IRQ_HANDLER name
    .weak \name
    .thumb_set \name, Default_Handler
.endm

IRQ_HANDLER NMI_Handler
IRQ_HANDLER HardFault_Handler
IRQ_HANDLER MemManage_Handler
IRQ_HANDLER BusFault_Handler
IRQ_HANDLER UsageFault_Handler
IRQ_HANDLER SVC_Handler
IRQ_HANDLER DebugMon_Handler
IRQ_HANDLER PendSV_Handler
IRQ_HANDLER SysTick_Handler

IRQ_HANDLER WWDG_IRQHandler
IRQ_HANDLER PVD_IRQHandler
IRQ_HANDLER TAMP_STAMP_IRQHandler
IRQ_HANDLER RTC_WKUP_IRQHandler
IRQ_HANDLER FLASH_IRQHandler
IRQ_HANDLER RCC_IRQHandler
IRQ_HANDLER EXTI0_IRQHandler
IRQ_HANDLER EXTI1_IRQHandler
IRQ_HANDLER EXTI2_IRQHandler
IRQ_HANDLER EXTI3_IRQHandler
IRQ_HANDLER EXTI4_IRQHandler
IRQ_HANDLER DMA1_Stream0_IRQHandler
IRQ_HANDLER DMA1_Stream1_IRQHandler
IRQ_HANDLER DMA1_Stream2_IRQHandler
IRQ_HANDLER DMA1_Stream3_IRQHandler
IRQ_HANDLER DMA1_Stream4_IRQHandler
IRQ_HANDLER DMA1_Stream5_IRQHandler
IRQ_HANDLER DMA1_Stream6_IRQHandler
IRQ_HANDLER ADC_IRQHandler
IRQ_HANDLER CAN1_TX_IRQHandler
IRQ_HANDLER CAN1_RX0_IRQHandler
IRQ_HANDLER CAN1_RX1_IRQHandler
IRQ_HANDLER CAN1_SCE_IRQHandler
IRQ_HANDLER EXTI9_5_IRQHandler
IRQ_HANDLER TIM1_BRK_TIM9_IRQHandler
IRQ_HANDLER TIM1_UP_TIM10_IRQHandler
IRQ_HANDLER TIM1_TRG_COM_TIM11_IRQHandler
IRQ_HANDLER TIM1_CC_IRQHandler
IRQ_HANDLER TIM2_IRQHandler
IRQ_HANDLER TIM3_IRQHandler
IRQ_HANDLER TIM4_IRQHandler
IRQ_HANDLER I2C1_EV_IRQHandler
IRQ_HANDLER I2C1_ER_IRQHandler
IRQ_HANDLER I2C2_EV_IRQHandler
IRQ_HANDLER I2C2_ER_IRQHandler
IRQ_HANDLER SPI1_IRQHandler
IRQ_HANDLER SPI2_IRQHandler
IRQ_HANDLER USART1_IRQHandler
IRQ_HANDLER USART2_IRQHandler
IRQ_HANDLER USART3_IRQHandler
IRQ_HANDLER EXTI15_10_IRQHandler
IRQ_HANDLER RTC_Alarm_IRQHandler
IRQ_HANDLER OTG_FS_WKUP_IRQHandler
IRQ_HANDLER TIM8_BRK_TIM12_IRQHandler
IRQ_HANDLER TIM8_UP_TIM13_IRQHandler
IRQ_HANDLER TIM8_TRG_COM_TIM14_IRQHandler
IRQ_HANDLER TIM8_CC_IRQHandler
IRQ_HANDLER DMA1_Stream7_IRQHandler
IRQ_HANDLER FMC_IRQHandler
IRQ_HANDLER SDIO_IRQHandler
IRQ_HANDLER TIM5_IRQHandler
IRQ_HANDLER SPI3_IRQHandler
IRQ_HANDLER UART4_IRQHandler
IRQ_HANDLER UART5_IRQHandler
IRQ_HANDLER TIM6_DAC_IRQHandler
IRQ_HANDLER TIM7_IRQHandler
IRQ_HANDLER DMA2_Stream0_IRQHandler
IRQ_HANDLER DMA2_Stream1_IRQHandler
IRQ_HANDLER DMA2_Stream2_IRQHandler
IRQ_HANDLER DMA2_Stream3_IRQHandler
IRQ_HANDLER DMA2_Stream4_IRQHandler
IRQ_HANDLER ETH_IRQHandler
IRQ_HANDLER ETH_WKUP_IRQHandler
IRQ_HANDLER CAN2_TX_IRQHandler
IRQ_HANDLER CAN2_RX0_IRQHandler
IRQ_HANDLER CAN2_RX1_IRQHandler
IRQ_HANDLER CAN2_SCE_IRQHandler
IRQ_HANDLER OTG_FS_IRQHandler
IRQ_HANDLER DMA2_Stream5_IRQHandler
IRQ_HANDLER DMA2_Stream6_IRQHandler
IRQ_HANDLER DMA2_Stream7_IRQHandler
IRQ_HANDLER USART6_IRQHandler
IRQ_HANDLER I2C3_EV_IRQHandler
IRQ_HANDLER I2C3_ER_IRQHandler
IRQ_HANDLER OTG_HS_EP1_OUT_IRQHandler
IRQ_HANDLER OTG_HS_EP1_IN_IRQHandler
IRQ_HANDLER OTG_HS_WKUP_IRQHandler
IRQ_HANDLER OTG_HS_IRQHandler
IRQ_HANDLER DCMI_IRQHandler
IRQ_HANDLER HASH_RNG_IRQHandler
IRQ_HANDLER FPU_IRQHandler
IRQ_HANDLER UART7_IRQHandler
IRQ_HANDLER UART8_IRQHandler
IRQ_HANDLER SPI4_IRQHandler
IRQ_HANDLER SPI5_IRQHandler
IRQ_HANDLER SPI6_IRQHandler
IRQ_HANDLER SAI1_IRQHandler
IRQ_HANDLER LTDC_IRQHandler
IRQ_HANDLER LTDC_ER_IRQHandler
IRQ_HANDLER DMA2D_IRQHandler
