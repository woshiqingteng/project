################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../BSP/app_ctrl.c \
../BSP/com_frame.c \
../BSP/drv_uart.c 

OBJS += \
./BSP/app_ctrl.o \
./BSP/com_frame.o \
./BSP/drv_uart.o 

C_DEPS += \
./BSP/app_ctrl.d \
./BSP/com_frame.d \
./BSP/drv_uart.d 


# Each subdirectory must supply rules for building sources it contributes
BSP/%.o BSP/%.su BSP/%.cyclo: ../BSP/%.c BSP/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F429xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../BSP -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-BSP

clean-BSP:
	-$(RM) ./BSP/app_ctrl.cyclo ./BSP/app_ctrl.d ./BSP/app_ctrl.o ./BSP/app_ctrl.su ./BSP/com_frame.cyclo ./BSP/com_frame.d ./BSP/com_frame.o ./BSP/com_frame.su ./BSP/drv_uart.cyclo ./BSP/drv_uart.d ./BSP/drv_uart.o ./BSP/drv_uart.su

.PHONY: clean-BSP

