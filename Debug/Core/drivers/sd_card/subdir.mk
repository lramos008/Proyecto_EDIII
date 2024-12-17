################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/drivers/sd_card/dsp_functions.c \
../Core/drivers/sd_card/sd_functions.c 

OBJS += \
./Core/drivers/sd_card/dsp_functions.o \
./Core/drivers/sd_card/sd_functions.o 

C_DEPS += \
./Core/drivers/sd_card/dsp_functions.d \
./Core/drivers/sd_card/sd_functions.d 


# Each subdirectory must supply rules for building sources it contributes
Core/drivers/sd_card/%.o Core/drivers/sd_card/%.su Core/drivers/sd_card/%.cyclo: ../Core/drivers/sd_card/%.c Core/drivers/sd_card/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xE -c -I"C:/Users/leora/OneDrive - Universidad de San Martin/Facultad/final_EDIII/Proyecto_EDIII/Core/drivers/keypad" -I"C:/Users/leora/OneDrive - Universidad de San Martin/Facultad/final_EDIII/Proyecto_EDIII/Core/drivers/sd_card" -I"C:/Users/leora/OneDrive - Universidad de San Martin/Facultad/final_EDIII/Proyecto_EDIII/Core/drivers/display" -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"C:/Users/leora/OneDrive - Universidad de San Martin/Facultad/final_EDIII/Proyecto_EDIII/Core/drivers/utils" -I"C:/Users/leora/OneDrive - Universidad de San Martin/Facultad/final_EDIII/Proyecto_EDIII/Core/subsystems" -I"C:/Users/leora/OneDrive - Universidad de San Martin/Facultad/final_EDIII/Proyecto_EDIII/Middlewares/fatfs" -I"C:/Users/leora/OneDrive - Universidad de San Martin/Facultad/final_EDIII/Proyecto_EDIII/Core/drivers/adc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-drivers-2f-sd_card

clean-Core-2f-drivers-2f-sd_card:
	-$(RM) ./Core/drivers/sd_card/dsp_functions.cyclo ./Core/drivers/sd_card/dsp_functions.d ./Core/drivers/sd_card/dsp_functions.o ./Core/drivers/sd_card/dsp_functions.su ./Core/drivers/sd_card/sd_functions.cyclo ./Core/drivers/sd_card/sd_functions.d ./Core/drivers/sd_card/sd_functions.o ./Core/drivers/sd_card/sd_functions.su

.PHONY: clean-Core-2f-drivers-2f-sd_card

