################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/drivers/adc/capture_voice_functions.c 

OBJS += \
./Core/drivers/adc/capture_voice_functions.o 

C_DEPS += \
./Core/drivers/adc/capture_voice_functions.d 


# Each subdirectory must supply rules for building sources it contributes
Core/drivers/adc/%.o Core/drivers/adc/%.su Core/drivers/adc/%.cyclo: ../Core/drivers/adc/%.c Core/drivers/adc/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xE -c -I"C:/Users/dramos/Documents/Final/Proyecto_EDIII/Core/drivers/keypad" -I"C:/Users/dramos/Documents/Final/Proyecto_EDIII/Core/drivers/sd_card" -I"C:/Users/dramos/Documents/Final/Proyecto_EDIII/Core/drivers/display" -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"C:/Users/dramos/Documents/Final/Proyecto_EDIII/Core/drivers/utils" -I"C:/Users/dramos/Documents/Final/Proyecto_EDIII/Core/subsystems" -I"C:/Users/dramos/Documents/Final/Proyecto_EDIII/Middlewares/fatfs" -I"C:/Users/dramos/Documents/Final/Proyecto_EDIII/Core/drivers/adc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-drivers-2f-adc

clean-Core-2f-drivers-2f-adc:
	-$(RM) ./Core/drivers/adc/capture_voice_functions.cyclo ./Core/drivers/adc/capture_voice_functions.d ./Core/drivers/adc/capture_voice_functions.o ./Core/drivers/adc/capture_voice_functions.su

.PHONY: clean-Core-2f-drivers-2f-adc

