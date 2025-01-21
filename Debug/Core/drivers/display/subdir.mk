################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/drivers/display/display_functions.c \
../Core/drivers/display/ssd1306.c \
../Core/drivers/display/ssd1306_fonts.c \
../Core/drivers/display/ssd1306_tests.c 

OBJS += \
./Core/drivers/display/display_functions.o \
./Core/drivers/display/ssd1306.o \
./Core/drivers/display/ssd1306_fonts.o \
./Core/drivers/display/ssd1306_tests.o 

C_DEPS += \
./Core/drivers/display/display_functions.d \
./Core/drivers/display/ssd1306.d \
./Core/drivers/display/ssd1306_fonts.d \
./Core/drivers/display/ssd1306_tests.d 


# Each subdirectory must supply rules for building sources it contributes
Core/drivers/display/%.o Core/drivers/display/%.su Core/drivers/display/%.cyclo: ../Core/drivers/display/%.c Core/drivers/display/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xE -c -I"C:/Users/dramos/Documents/Final/Proyecto_EDIII/Core/subsystems/file_handling" -I"C:/Users/dramos/Documents/Final/Proyecto_EDIII/Core/subsystems/access_control" -I"C:/Users/dramos/Documents/Final/Proyecto_EDIII/Core/subsystems/comparing" -I"C:/Users/dramos/Documents/Final/Proyecto_EDIII/Core/subsystems/feature_extraction" -I"C:/Users/dramos/Documents/Final/Proyecto_EDIII/Core/subsystems/handle_digit_logic" -I"C:/Users/dramos/Documents/Final/Proyecto_EDIII/Core/subsystems/processing" -I"C:/Users/dramos/Documents/Final/Proyecto_EDIII/Core/subsystems/template_creation" -I"C:/Users/dramos/Documents/Final/Proyecto_EDIII/Core/subsystems/voice_capture" -I"C:/Users/dramos/Documents/Final/Proyecto_EDIII/Core/subsystems/voice_recognition" -I"C:/Users/dramos/Documents/Final/Proyecto_EDIII/Core/drivers/keypad" -I"C:/Users/dramos/Documents/Final/Proyecto_EDIII/Core/drivers/sd_card" -I"C:/Users/dramos/Documents/Final/Proyecto_EDIII/Core/drivers/display" -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"C:/Users/dramos/Documents/Final/Proyecto_EDIII/Core/subsystems" -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -I"C:/Users/dramos/Documents/Final/Proyecto_EDIII/Core/utils" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-drivers-2f-display

clean-Core-2f-drivers-2f-display:
	-$(RM) ./Core/drivers/display/display_functions.cyclo ./Core/drivers/display/display_functions.d ./Core/drivers/display/display_functions.o ./Core/drivers/display/display_functions.su ./Core/drivers/display/ssd1306.cyclo ./Core/drivers/display/ssd1306.d ./Core/drivers/display/ssd1306.o ./Core/drivers/display/ssd1306.su ./Core/drivers/display/ssd1306_fonts.cyclo ./Core/drivers/display/ssd1306_fonts.d ./Core/drivers/display/ssd1306_fonts.o ./Core/drivers/display/ssd1306_fonts.su ./Core/drivers/display/ssd1306_tests.cyclo ./Core/drivers/display/ssd1306_tests.d ./Core/drivers/display/ssd1306_tests.o ./Core/drivers/display/ssd1306_tests.su

.PHONY: clean-Core-2f-drivers-2f-display

