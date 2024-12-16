################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/subsystems/adc_task.c \
../Core/subsystems/display_task.c \
../Core/subsystems/keypad_task.c \
../Core/subsystems/sd_task.c 

OBJS += \
./Core/subsystems/adc_task.o \
./Core/subsystems/display_task.o \
./Core/subsystems/keypad_task.o \
./Core/subsystems/sd_task.o 

C_DEPS += \
./Core/subsystems/adc_task.d \
./Core/subsystems/display_task.d \
./Core/subsystems/keypad_task.d \
./Core/subsystems/sd_task.d 


# Each subdirectory must supply rules for building sources it contributes
Core/subsystems/%.o Core/subsystems/%.su Core/subsystems/%.cyclo: ../Core/subsystems/%.c Core/subsystems/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xE -c -I"C:/Users/dramos/Documents/Final/Proyecto_EDIII/Core/drivers/keypad" -I"C:/Users/dramos/Documents/Final/Proyecto_EDIII/Core/drivers/sd_card" -I"C:/Users/dramos/Documents/Final/Proyecto_EDIII/Core/drivers/display" -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"C:/Users/dramos/Documents/Final/Proyecto_EDIII/Core/drivers/utils" -I"C:/Users/dramos/Documents/Final/Proyecto_EDIII/Core/subsystems" -I"C:/Users/dramos/Documents/Final/Proyecto_EDIII/Middlewares/fatfs" -I"C:/Users/dramos/Documents/Final/Proyecto_EDIII/Core/drivers/adc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-subsystems

clean-Core-2f-subsystems:
	-$(RM) ./Core/subsystems/adc_task.cyclo ./Core/subsystems/adc_task.d ./Core/subsystems/adc_task.o ./Core/subsystems/adc_task.su ./Core/subsystems/display_task.cyclo ./Core/subsystems/display_task.d ./Core/subsystems/display_task.o ./Core/subsystems/display_task.su ./Core/subsystems/keypad_task.cyclo ./Core/subsystems/keypad_task.d ./Core/subsystems/keypad_task.o ./Core/subsystems/keypad_task.su ./Core/subsystems/sd_task.cyclo ./Core/subsystems/sd_task.d ./Core/subsystems/sd_task.o ./Core/subsystems/sd_task.su

.PHONY: clean-Core-2f-subsystems

