################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/TFT/fonts.c \
../Drivers/TFT/ili9341.c \
../Drivers/TFT/testimg.c 

OBJS += \
./Drivers/TFT/fonts.o \
./Drivers/TFT/ili9341.o \
./Drivers/TFT/testimg.o 

C_DEPS += \
./Drivers/TFT/fonts.d \
./Drivers/TFT/ili9341.d \
./Drivers/TFT/testimg.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/TFT/%.o Drivers/TFT/%.su Drivers/TFT/%.cyclo: ../Drivers/TFT/%.c Drivers/TFT/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/taher/STM32CubeIDE/workspace_lvgl/Handheld-Nucleo/Drivers/GAME" -I"C:/Users/taher/STM32CubeIDE/workspace_lvgl/Handheld-Nucleo/Drivers/GAME2" -I"C:/Users/taher/STM32CubeIDE/workspace_lvgl/Handheld-Nucleo/Drivers/MPU6050" -I"C:/Users/taher/STM32CubeIDE/workspace_lvgl/Handheld-Nucleo/Drivers/TFT" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-TFT

clean-Drivers-2f-TFT:
	-$(RM) ./Drivers/TFT/fonts.cyclo ./Drivers/TFT/fonts.d ./Drivers/TFT/fonts.o ./Drivers/TFT/fonts.su ./Drivers/TFT/ili9341.cyclo ./Drivers/TFT/ili9341.d ./Drivers/TFT/ili9341.o ./Drivers/TFT/ili9341.su ./Drivers/TFT/testimg.cyclo ./Drivers/TFT/testimg.d ./Drivers/TFT/testimg.o ./Drivers/TFT/testimg.su

.PHONY: clean-Drivers-2f-TFT

