################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include protocol/bluetooth_2.7/lib/EFR32BG1B/GCC/subdir.mk
-include platform/emlib/src/subdir.mk
-include platform/emdrv/tempdrv/src/subdir.mk
-include platform/emdrv/sleep/src/subdir.mk
-include platform/Device/SiliconLabs/EFR32BG1B/Source/GCC/subdir.mk
-include platform/Device/SiliconLabs/EFR32BG1B/Source/subdir.mk
-include hardware/kit/common/bsp/subdir.mk
-include User/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: botao_panico_custom_board.axf

# Tool invocations
botao_panico_custom_board.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU ARM C Linker'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m4 -mthumb -T "C:\Users\bruno\SimplicityStudio\v4_workspace\botao_panico_custom_board\efr32bg1b132f256gm32.ld" -Xlinker --gc-sections -Xlinker -Map="botao_panico_custom_board.map" -mfpu=fpv4-sp-d16 -mfloat-abi=softfp --specs=nano.specs -o botao_panico_custom_board.axf -Wl,--start-group "./User/functions.o" "./User/my_adc.o" "./User/my_dma.o" "./User/my_gpio.o" "./User/my_ibeacon.o" "./User/my_init.o" "./User/my_leusart.o" "./User/my_usart.o" "./application_properties.o" "./bsp_trace.o" "./custom_adv.o" "./eeprom_emulation.o" "./gatt_db.o" "./init_app.o" "./init_board.o" "./init_mcu.o" "./interrupt.o" "./main.o" "./hardware/kit/common/bsp/bsp_stk.o" "./platform/Device/SiliconLabs/EFR32BG1B/Source/GCC/startup_efr32bg1b.o" "./platform/Device/SiliconLabs/EFR32BG1B/Source/system_efr32bg1b.o" "./platform/emdrv/sleep/src/sleep.o" "./platform/emdrv/tempdrv/src/tempdrv.o" "./platform/emlib/src/em_cmu.o" "./platform/emlib/src/em_core.o" "./platform/emlib/src/em_cryotimer.o" "./platform/emlib/src/em_crypto.o" "./platform/emlib/src/em_emu.o" "./platform/emlib/src/em_gpio.o" "./platform/emlib/src/em_i2c.o" "./platform/emlib/src/em_msc.o" "./platform/emlib/src/em_rmu.o" "./platform/emlib/src/em_rtcc.o" "./platform/emlib/src/em_system.o" "./platform/emlib/src/em_timer.o" "./platform/emlib/src/em_usart.o" "C:\Users\bruno\SimplicityStudio\v4_workspace\botao_panico_custom_board\platform\radio\rail_lib\autogen\librail_release\librail_efr32xg1_gcc_release.a" "C:\Users\bruno\SimplicityStudio\v4_workspace\botao_panico_custom_board\protocol\bluetooth_2.7\lib\EFR32BG1B\GCC\libbluetooth.a" "C:\Users\bruno\SimplicityStudio\v4_workspace\botao_panico_custom_board\protocol\bluetooth_2.7\lib\EFR32BG1B\GCC\binapploader.o" "C:\Users\bruno\SimplicityStudio\v4_workspace\botao_panico_custom_board\protocol\bluetooth_2.7\lib\EFR32BG1B\GCC\libpsstore.a" "C:\Users\bruno\SimplicityStudio\v4_workspace\botao_panico_custom_board\protocol\bluetooth_2.7\lib\EFR32BG1B\GCC\libmbedtls.a" -lm -Wl,--end-group -Wl,--start-group -lgcc -lc -lnosys -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

	@echo 'Building hex file: botao_panico_custom_board.hex'
	arm-none-eabi-objcopy -O ihex "botao_panico_custom_board.axf" "botao_panico_custom_board.hex"
	@echo ' '

	@echo 'Building bin file: botao_panico_custom_board.bin'
	arm-none-eabi-objcopy -O binary "botao_panico_custom_board.axf" "botao_panico_custom_board.bin"
	@echo ' '

	@echo 'Building s37 file: botao_panico_custom_board.s37'
	arm-none-eabi-objcopy -O srec "botao_panico_custom_board.axf" "botao_panico_custom_board.s37"
	@echo ' '

	@echo 'Running size tool'
	arm-none-eabi-size "botao_panico_custom_board.axf"
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) botao_panico_custom_board.axf
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
