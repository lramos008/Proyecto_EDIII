################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/drivers/sd_card/save_and_read_data.c \
../Core/drivers/sd_card/sd_functions.c \
../Core/drivers/sd_card/user_and_entry.c 

OBJS += \
./Core/drivers/sd_card/save_and_read_data.o \
./Core/drivers/sd_card/sd_functions.o \
./Core/drivers/sd_card/user_and_entry.o 

C_DEPS += \
./Core/drivers/sd_card/save_and_read_data.d \
./Core/drivers/sd_card/sd_functions.d \
./Core/drivers/sd_card/user_and_entry.d 


# Each subdirectory must supply rules for building sources it contributes
Core/drivers/sd_card/%.o Core/drivers/sd_card/%.su Core/drivers/sd_card/%.cyclo: ../Core/drivers/sd_card/%.c Core/drivers/sd_card/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xE -c -I"C:/Users/dramos/Documents/Final/Proyecto_EDIII/Core/subsystems/file_handling" -I"C:/Users/dramos/Documents/Final/Proyecto_EDIII/Core/subsystems/access_control" -I"C:/Users/dramos/Documents/Final/Proyecto_EDIII/Core/subsystems/comparing" -I"C:/Users/dramos/Documents/Final/Proyecto_EDIII/Core/subsystems/feature_extraction" -I"C:/Users/dramos/Documents/Final/Proyecto_EDIII/Core/subsystems/handle_digit_logic" -I"C:/Users/dramos/Documents/Final/Proyecto_EDIII/Core/subsystems/processing" -I"C:/Users/dramos/Documents/Final/Proyecto_EDIII/Core/subsystems/template_creation" -I"C:/Users/dramos/Documents/Final/Proyecto_EDIII/Core/subsystems/voice_capture" -I"C:/Users/dramos/Documents/Final/Proyecto_EDIII/Core/subsystems/voice_recognition" -I"C:/Users/dramos/Documents/Final/Proyecto_EDIII/Core/drivers/keypad" -I"C:/Users/dramos/Documents/Final/Proyecto_EDIII/Core/drivers/sd_card" -I"C:/Users/dramos/Documents/Final/Proyecto_EDIII/Core/drivers/display" -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"C:/Users/dramos/Documents/Final/Proyecto_EDIII/Core/subsystems" -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -I"C:/Users/dramos/Documents/Final/Proyecto_EDIII/Core/utils" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-drivers-2f-sd_card

clean-Core-2f-drivers-2f-sd_card:
	-$(RM) ./Core/drivers/sd_card/save_and_read_data.cyclo ./Core/drivers/sd_card/save_and_read_data.d ./Core/drivers/sd_card/save_and_read_data.o ./Core/drivers/sd_card/save_and_read_data.su ./Core/drivers/sd_card/sd_functions.cyclo ./Core/drivers/sd_card/sd_functions.d ./Core/drivers/sd_card/sd_functions.o ./Core/drivers/sd_card/sd_functions.su ./Core/drivers/sd_card/user_and_entry.cyclo ./Core/drivers/sd_card/user_and_entry.d ./Core/drivers/sd_card/user_and_entry.o ./Core/drivers/sd_card/user_and_entry.su

.PHONY: clean-Core-2f-drivers-2f-sd_card

