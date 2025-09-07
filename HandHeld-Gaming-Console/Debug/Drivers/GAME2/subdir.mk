################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/GAME2/pingpong.c 

OBJS += \
./Drivers/GAME2/pingpong.o 

C_DEPS += \
./Drivers/GAME2/pingpong.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/GAME2/%.o Drivers/GAME2/%.su Drivers/GAME2/%.cyclo: ../Drivers/GAME2/%.c Drivers/GAME2/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/taher/STM32CubeIDE/workspace_lvgl/HandHeld-Gaming-Console/Drivers/GAME" -I"C:/Users/taher/STM32CubeIDE/workspace_lvgl/HandHeld-Gaming-Console/Drivers/GAME2" -I"C:/Users/taher/STM32CubeIDE/workspace_lvgl/HandHeld-Gaming-Console/Drivers/MPU6050" -I"C:/Users/taher/STM32CubeIDE/workspace_lvgl/HandHeld-Gaming-Console/Drivers/TFT" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-GAME2

clean-Drivers-2f-GAME2:
	-$(RM) ./Drivers/GAME2/pingpong.cyclo ./Drivers/GAME2/pingpong.d ./Drivers/GAME2/pingpong.o ./Drivers/GAME2/pingpong.su

.PHONY: clean-Drivers-2f-GAME2

