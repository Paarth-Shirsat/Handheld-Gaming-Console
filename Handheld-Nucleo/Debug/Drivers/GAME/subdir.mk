################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/GAME/snake.c 

OBJS += \
./Drivers/GAME/snake.o 

C_DEPS += \
./Drivers/GAME/snake.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/GAME/%.o Drivers/GAME/%.su Drivers/GAME/%.cyclo: ../Drivers/GAME/%.c Drivers/GAME/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/taher/STM32CubeIDE/workspace_lvgl/Handheld-Nucleo/Drivers/GAME" -I"C:/Users/taher/STM32CubeIDE/workspace_lvgl/Handheld-Nucleo/Drivers/GAME2" -I"C:/Users/taher/STM32CubeIDE/workspace_lvgl/Handheld-Nucleo/Drivers/MPU6050" -I"C:/Users/taher/STM32CubeIDE/workspace_lvgl/Handheld-Nucleo/Drivers/TFT" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-GAME

clean-Drivers-2f-GAME:
	-$(RM) ./Drivers/GAME/snake.cyclo ./Drivers/GAME/snake.d ./Drivers/GAME/snake.o ./Drivers/GAME/snake.su

.PHONY: clean-Drivers-2f-GAME

