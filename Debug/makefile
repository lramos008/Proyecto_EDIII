################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/subdir.mk
-include Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F/subdir.mk
-include Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS/subdir.mk
-include Middlewares/Third_Party/FreeRTOS/Source/subdir.mk
-include Middlewares/Third_Party/FatFs/src/option/subdir.mk
-include Middlewares/Third_Party/FatFs/src/subdir.mk
-include FATFS/Target/subdir.mk
-include FATFS/App/subdir.mk
-include Drivers/STM32F4xx_HAL_Driver/Src/subdir.mk
-include Core/utils/subdir.mk
-include Core/tasks/subdir.mk
-include Core/subsystems/voice_recognition/subdir.mk
-include Core/subsystems/voice_capture/subdir.mk
-include Core/subsystems/template_creation/subdir.mk
-include Core/subsystems/processing/subdir.mk
-include Core/subsystems/handle_digit_logic/subdir.mk
-include Core/subsystems/file_handling/subdir.mk
-include Core/subsystems/feature_extraction/subdir.mk
-include Core/subsystems/comparing/subdir.mk
-include Core/subsystems/access_control/subdir.mk
-include Core/drivers/sd_card/subdir.mk
-include Core/drivers/keypad/subdir.mk
-include Core/drivers/display/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := EDIII_Control_Acceso
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
EDIII_Control_Acceso.elf \

MAP_FILES += \
EDIII_Control_Acceso.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
EDIII_Control_Acceso.list \


# All Target
all: main-build

# Main-build Target
main-build: EDIII_Control_Acceso.elf secondary-outputs

# Tool invocations
EDIII_Control_Acceso.elf EDIII_Control_Acceso.map: $(OBJS) $(USER_OBJS) C:\Users\leora\OneDrive\ -\ Universidad\ de\ San\ Martin\Facultad\final_EDIII\Proyecto_EDIII\STM32F401RETX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "EDIII_Control_Acceso.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"C:\Users\leora\OneDrive - Universidad de San Martin\Facultad\final_EDIII\Proyecto_EDIII\STM32F401RETX_FLASH.ld" --specs=nosys.specs -Wl,-Map="EDIII_Control_Acceso.map" -Wl,--gc-sections -static -L"C:\Users\leora\OneDrive - Universidad de San Martin\Facultad\final_EDIII\Proyecto_EDIII\Lib" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

EDIII_Control_Acceso.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "EDIII_Control_Acceso.list"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) EDIII_Control_Acceso.elf EDIII_Control_Acceso.list EDIII_Control_Acceso.map default.size.stdout
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets
