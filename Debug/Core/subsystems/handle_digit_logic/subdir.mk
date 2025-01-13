################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/subsystems/handle_digit_logic/digit_handle_logic.c 

OBJS += \
./Core/subsystems/handle_digit_logic/digit_handle_logic.o 

C_DEPS += \
./Core/subsystems/handle_digit_logic/digit_handle_logic.d 


# Each subdirectory must supply rules for building sources it contributes
Core/subsystems/handle_digit_logic/%.o Core/subsystems/handle_digit_logic/%.su Core/subsystems/handle_digit_logic/%.cyclo: ../Core/subsystems/handle_digit_logic/%.c Core/subsystems/handle_digit_logic/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xE -c -I"C:/Users/leora/OneDrive - Universidad de San Martin/Facultad/final_EDIII/Proyecto_EDIII/Core/subsystems/file_handling" -I"C:/Users/leora/OneDrive - Universidad de San Martin/Facultad/final_EDIII/Proyecto_EDIII/Core/subsystems/access_control" -I"C:/Users/leora/OneDrive - Universidad de San Martin/Facultad/final_EDIII/Proyecto_EDIII/Core/subsystems/comparing" -I"C:/Users/leora/OneDrive - Universidad de San Martin/Facultad/final_EDIII/Proyecto_EDIII/Core/subsystems/feature_extraction" -I"C:/Users/leora/OneDrive - Universidad de San Martin/Facultad/final_EDIII/Proyecto_EDIII/Core/subsystems/handle_digit_logic" -I"C:/Users/leora/OneDrive - Universidad de San Martin/Facultad/final_EDIII/Proyecto_EDIII/Core/subsystems/processing" -I"C:/Users/leora/OneDrive - Universidad de San Martin/Facultad/final_EDIII/Proyecto_EDIII/Core/subsystems/template_creation" -I"C:/Users/leora/OneDrive - Universidad de San Martin/Facultad/final_EDIII/Proyecto_EDIII/Core/subsystems/voice_capture" -I"C:/Users/leora/OneDrive - Universidad de San Martin/Facultad/final_EDIII/Proyecto_EDIII/Core/subsystems/voice_recognition" -I"C:/Users/leora/OneDrive - Universidad de San Martin/Facultad/final_EDIII/Proyecto_EDIII/Core/drivers/keypad" -I"C:/Users/leora/OneDrive - Universidad de San Martin/Facultad/final_EDIII/Proyecto_EDIII/Core/drivers/sd_card" -I"C:/Users/leora/OneDrive - Universidad de San Martin/Facultad/final_EDIII/Proyecto_EDIII/Core/drivers/display" -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"C:/Users/leora/OneDrive - Universidad de San Martin/Facultad/final_EDIII/Proyecto_EDIII/Core/subsystems" -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -I"C:/Users/leora/OneDrive - Universidad de San Martin/Facultad/final_EDIII/Proyecto_EDIII/Core/utils" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-subsystems-2f-handle_digit_logic

clean-Core-2f-subsystems-2f-handle_digit_logic:
	-$(RM) ./Core/subsystems/handle_digit_logic/digit_handle_logic.cyclo ./Core/subsystems/handle_digit_logic/digit_handle_logic.d ./Core/subsystems/handle_digit_logic/digit_handle_logic.o ./Core/subsystems/handle_digit_logic/digit_handle_logic.su

.PHONY: clean-Core-2f-subsystems-2f-handle_digit_logic

