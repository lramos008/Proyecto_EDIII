################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/utils/common_utils.c 

OBJS += \
./Core/utils/common_utils.o 

C_DEPS += \
./Core/utils/common_utils.d 


# Each subdirectory must supply rules for building sources it contributes
Core/utils/%.o Core/utils/%.su Core/utils/%.cyclo: ../Core/utils/%.c Core/utils/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xE -c -I"C:/Users/dramos/Documents/Final/Proyecto_EDIII/Core/drivers/keypad" -I"C:/Users/dramos/Documents/Final/Proyecto_EDIII/Core/drivers/sd_card" -I"C:/Users/dramos/Documents/Final/Proyecto_EDIII/Core/drivers/display" -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -I"C:/Users/dramos/Documents/Final/Proyecto_EDIII/Core/utils" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-utils

clean-Core-2f-utils:
	-$(RM) ./Core/utils/common_utils.cyclo ./Core/utils/common_utils.d ./Core/utils/common_utils.o ./Core/utils/common_utils.su

.PHONY: clean-Core-2f-utils

