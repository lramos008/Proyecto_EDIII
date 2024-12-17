################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/fatfs/diskio.c \
../Middlewares/fatfs/fatfs_sd.c \
../Middlewares/fatfs/ff.c \
../Middlewares/fatfs/ffsystem.c \
../Middlewares/fatfs/ffunicode.c 

OBJS += \
./Middlewares/fatfs/diskio.o \
./Middlewares/fatfs/fatfs_sd.o \
./Middlewares/fatfs/ff.o \
./Middlewares/fatfs/ffsystem.o \
./Middlewares/fatfs/ffunicode.o 

C_DEPS += \
./Middlewares/fatfs/diskio.d \
./Middlewares/fatfs/fatfs_sd.d \
./Middlewares/fatfs/ff.d \
./Middlewares/fatfs/ffsystem.d \
./Middlewares/fatfs/ffunicode.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/fatfs/%.o Middlewares/fatfs/%.su Middlewares/fatfs/%.cyclo: ../Middlewares/fatfs/%.c Middlewares/fatfs/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xE -c -I"C:/Users/leora/OneDrive - Universidad de San Martin/Facultad/final_EDIII/Proyecto_EDIII/Core/drivers/keypad" -I"C:/Users/leora/OneDrive - Universidad de San Martin/Facultad/final_EDIII/Proyecto_EDIII/Core/drivers/sd_card" -I"C:/Users/leora/OneDrive - Universidad de San Martin/Facultad/final_EDIII/Proyecto_EDIII/Core/drivers/display" -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"C:/Users/leora/OneDrive - Universidad de San Martin/Facultad/final_EDIII/Proyecto_EDIII/Core/drivers/utils" -I"C:/Users/leora/OneDrive - Universidad de San Martin/Facultad/final_EDIII/Proyecto_EDIII/Core/subsystems" -I"C:/Users/leora/OneDrive - Universidad de San Martin/Facultad/final_EDIII/Proyecto_EDIII/Middlewares/fatfs" -I"C:/Users/leora/OneDrive - Universidad de San Martin/Facultad/final_EDIII/Proyecto_EDIII/Core/drivers/adc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Middlewares-2f-fatfs

clean-Middlewares-2f-fatfs:
	-$(RM) ./Middlewares/fatfs/diskio.cyclo ./Middlewares/fatfs/diskio.d ./Middlewares/fatfs/diskio.o ./Middlewares/fatfs/diskio.su ./Middlewares/fatfs/fatfs_sd.cyclo ./Middlewares/fatfs/fatfs_sd.d ./Middlewares/fatfs/fatfs_sd.o ./Middlewares/fatfs/fatfs_sd.su ./Middlewares/fatfs/ff.cyclo ./Middlewares/fatfs/ff.d ./Middlewares/fatfs/ff.o ./Middlewares/fatfs/ff.su ./Middlewares/fatfs/ffsystem.cyclo ./Middlewares/fatfs/ffsystem.d ./Middlewares/fatfs/ffsystem.o ./Middlewares/fatfs/ffsystem.su ./Middlewares/fatfs/ffunicode.cyclo ./Middlewares/fatfs/ffunicode.d ./Middlewares/fatfs/ffunicode.o ./Middlewares/fatfs/ffunicode.su

.PHONY: clean-Middlewares-2f-fatfs

