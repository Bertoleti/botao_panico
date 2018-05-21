################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../platform/emdrv/tempdrv/src/tempdrv.c 

OBJS += \
./platform/emdrv/tempdrv/src/tempdrv.o 

C_DEPS += \
./platform/emdrv/tempdrv/src/tempdrv.d 


# Each subdirectory must supply rules for building sources it contributes
platform/emdrv/tempdrv/src/tempdrv.o: ../platform/emdrv/tempdrv/src/tempdrv.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DHAL_CONFIG=1' '-D__HEAP_SIZE=0xD00' '-DRETARGET_VCOM=1' '-D__STACK_SIZE=0x800' '-DEFR32BG1B132F256GM32=1' -I"C:\Users\Bruno\SimplicityStudio\v4_workspace\botao_panico_custom_board\platform\Device\SiliconLabs\EFR32BG1B\Source\GCC" -I"C:\Users\Bruno\SimplicityStudio\v4_workspace\botao_panico_custom_board\platform\halconfig\inc\hal-config" -I"C:\Users\Bruno\SimplicityStudio\v4_workspace\botao_panico_custom_board\platform\emdrv\uartdrv\inc" -I"C:\Users\Bruno\SimplicityStudio\v4_workspace\botao_panico_custom_board\platform\bootloader\api" -I"C:\Users\Bruno\SimplicityStudio\v4_workspace\botao_panico_custom_board\hardware\kit\common\drivers" -I"C:\Users\Bruno\SimplicityStudio\v4_workspace\botao_panico_custom_board\platform\radio\rail_lib\common" -I"C:\Users\Bruno\SimplicityStudio\v4_workspace\botao_panico_custom_board\platform\emdrv\tempdrv\src" -I"C:\Users\Bruno\SimplicityStudio\v4_workspace\botao_panico_custom_board\platform\emdrv\sleep\inc" -I"C:\Users\Bruno\SimplicityStudio\v4_workspace\botao_panico_custom_board" -I"C:\Users\Bruno\SimplicityStudio\v4_workspace\botao_panico_custom_board\protocol\bluetooth_2.7\ble_stack\inc\soc" -I"C:\Users\Bruno\SimplicityStudio\v4_workspace\botao_panico_custom_board\platform\emdrv\common\inc" -I"C:\Users\Bruno\SimplicityStudio\v4_workspace\botao_panico_custom_board\protocol\bluetooth_2.7\ble_stack\inc\common" -I"C:\Users\Bruno\SimplicityStudio\v4_workspace\botao_panico_custom_board\app\bluetooth_2.7\common\stack_bridge" -I"C:\Users\Bruno\SimplicityStudio\v4_workspace\botao_panico_custom_board\platform\emdrv\sleep\src" -I"C:\Users\Bruno\SimplicityStudio\v4_workspace\botao_panico_custom_board\platform\emlib\src" -I"C:\Users\Bruno\SimplicityStudio\v4_workspace\botao_panico_custom_board\platform\Device\SiliconLabs\EFR32BG1B\Source" -I"C:\Users\Bruno\SimplicityStudio\v4_workspace\botao_panico_custom_board\platform\radio\rail_lib\chip\efr32" -I"C:\Users\Bruno\SimplicityStudio\v4_workspace\botao_panico_custom_board\platform\emlib\inc" -I"C:\Users\Bruno\SimplicityStudio\v4_workspace\botao_panico_custom_board\platform\bootloader" -I"C:\Users\Bruno\SimplicityStudio\v4_workspace\botao_panico_custom_board\platform\CMSIS\Include" -I"C:\Users\Bruno\SimplicityStudio\v4_workspace\botao_panico_custom_board\platform\emdrv\tempdrv\inc" -I"C:\Users\Bruno\SimplicityStudio\v4_workspace\botao_panico_custom_board\platform\emdrv\gpiointerrupt\inc" -I"C:\Users\Bruno\SimplicityStudio\v4_workspace\botao_panico_custom_board\platform\Device\SiliconLabs\EFR32BG1B\Include" -I"C:\Users\Bruno\SimplicityStudio\v4_workspace\botao_panico_custom_board\hardware\kit\common\bsp" -I"C:\Users\Bruno\SimplicityStudio\v4_workspace\botao_panico_custom_board\hardware\kit\common\halconfig" -I"C:\Users\Bruno\SimplicityStudio\v4_workspace\botao_panico_custom_board\platform\emdrv\dmadrv\inc" -O2 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -MMD -MP -MF"platform/emdrv/tempdrv/src/tempdrv.d" -MT"platform/emdrv/tempdrv/src/tempdrv.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


