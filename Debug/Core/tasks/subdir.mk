################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/tasks/display_task.c \
../Core/tasks/keypad_task.c \
../Core/tasks/sd_task.c 

OBJS += \
./Core/tasks/display_task.o \
./Core/tasks/keypad_task.o \
./Core/tasks/sd_task.o 

C_DEPS += \
./Core/tasks/display_task.d \
./Core/tasks/keypad_task.d \
./Core/tasks/sd_task.d 


# Each subdirectory must supply rules for building sources it contributes
Core/tasks/%.o Core/tasks/%.su Core/tasks/%.cyclo: ../Core/tasks/%.c Core/tasks/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xE -c -I"C:/Users/Juli-Leo/Documents/Facultad/Proyecto_EDIII/Core/drivers/keypad" -I"C:/Users/Juli-Leo/Documents/Facultad/Proyecto_EDIII/Core/drivers/sd_card" -I"C:/Users/Juli-Leo/Documents/Facultad/Proyecto_EDIII/Core/drivers/display" -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -I"C:/Users/Juli-Leo/Documents/Facultad/Proyecto_EDIII/Core/utils" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-tasks

clean-Core-2f-tasks:
	-$(RM) ./Core/tasks/display_task.cyclo ./Core/tasks/display_task.d ./Core/tasks/display_task.o ./Core/tasks/display_task.su ./Core/tasks/keypad_task.cyclo ./Core/tasks/keypad_task.d ./Core/tasks/keypad_task.o ./Core/tasks/keypad_task.su ./Core/tasks/sd_task.cyclo ./Core/tasks/sd_task.d ./Core/tasks/sd_task.o ./Core/tasks/sd_task.su

.PHONY: clean-Core-2f-tasks

