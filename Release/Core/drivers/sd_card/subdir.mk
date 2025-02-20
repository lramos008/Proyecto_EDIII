################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/drivers/sd_card/capture_and_store.c \
../Core/drivers/sd_card/processing.c \
../Core/drivers/sd_card/sd_functions.c \
../Core/drivers/sd_card/sd_utils.c 

OBJS += \
./Core/drivers/sd_card/capture_and_store.o \
./Core/drivers/sd_card/processing.o \
./Core/drivers/sd_card/sd_functions.o \
./Core/drivers/sd_card/sd_utils.o 

C_DEPS += \
./Core/drivers/sd_card/capture_and_store.d \
./Core/drivers/sd_card/processing.d \
./Core/drivers/sd_card/sd_functions.d \
./Core/drivers/sd_card/sd_utils.d 


# Each subdirectory must supply rules for building sources it contributes
Core/drivers/sd_card/%.o Core/drivers/sd_card/%.su Core/drivers/sd_card/%.cyclo: ../Core/drivers/sd_card/%.c Core/drivers/sd_card/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32F401xE -c -I../Core/Inc -I"C:/Users/Juli-Leo/Documents/Facultad/Proyecto_EDIII/Core/drivers/display" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I"C:/Users/Juli-Leo/Documents/Facultad/Proyecto_EDIII/Core/utils" -I"C:/Users/Juli-Leo/Documents/Facultad/Proyecto_EDIII/Core/drivers/keypad" -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../FATFS/Target -I../FATFS/App -I"C:/Users/Juli-Leo/Documents/Facultad/Proyecto_EDIII/Core/drivers/sd_card" -I../Middlewares/Third_Party/FatFs/src -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-drivers-2f-sd_card

clean-Core-2f-drivers-2f-sd_card:
	-$(RM) ./Core/drivers/sd_card/capture_and_store.cyclo ./Core/drivers/sd_card/capture_and_store.d ./Core/drivers/sd_card/capture_and_store.o ./Core/drivers/sd_card/capture_and_store.su ./Core/drivers/sd_card/processing.cyclo ./Core/drivers/sd_card/processing.d ./Core/drivers/sd_card/processing.o ./Core/drivers/sd_card/processing.su ./Core/drivers/sd_card/sd_functions.cyclo ./Core/drivers/sd_card/sd_functions.d ./Core/drivers/sd_card/sd_functions.o ./Core/drivers/sd_card/sd_functions.su ./Core/drivers/sd_card/sd_utils.cyclo ./Core/drivers/sd_card/sd_utils.d ./Core/drivers/sd_card/sd_utils.o ./Core/drivers/sd_card/sd_utils.su

.PHONY: clean-Core-2f-drivers-2f-sd_card

