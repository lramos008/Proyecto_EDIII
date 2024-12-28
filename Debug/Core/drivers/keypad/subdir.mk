################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/drivers/keypad/digit_handle_logic.c \
../Core/drivers/keypad/keypad_functions.c 

OBJS += \
./Core/drivers/keypad/digit_handle_logic.o \
./Core/drivers/keypad/keypad_functions.o 

C_DEPS += \
./Core/drivers/keypad/digit_handle_logic.d \
./Core/drivers/keypad/keypad_functions.d 


# Each subdirectory must supply rules for building sources it contributes
Core/drivers/keypad/%.o Core/drivers/keypad/%.su Core/drivers/keypad/%.cyclo: ../Core/drivers/keypad/%.c Core/drivers/keypad/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xE -c -I"C:/Users/leora/OneDrive - Universidad de San Martin/Facultad/final_EDIII/Proyecto_EDIII/Core/drivers/keypad" -I"C:/Users/leora/OneDrive - Universidad de San Martin/Facultad/final_EDIII/Proyecto_EDIII/Core/drivers/sd_card" -I"C:/Users/leora/OneDrive - Universidad de San Martin/Facultad/final_EDIII/Proyecto_EDIII/Core/drivers/display" -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -I"C:/Users/leora/OneDrive - Universidad de San Martin/Facultad/final_EDIII/Proyecto_EDIII/Core/utils" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-drivers-2f-keypad

clean-Core-2f-drivers-2f-keypad:
	-$(RM) ./Core/drivers/keypad/digit_handle_logic.cyclo ./Core/drivers/keypad/digit_handle_logic.d ./Core/drivers/keypad/digit_handle_logic.o ./Core/drivers/keypad/digit_handle_logic.su ./Core/drivers/keypad/keypad_functions.cyclo ./Core/drivers/keypad/keypad_functions.d ./Core/drivers/keypad/keypad_functions.o ./Core/drivers/keypad/keypad_functions.su

.PHONY: clean-Core-2f-drivers-2f-keypad

