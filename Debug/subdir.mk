################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../cpi_code.c 

OBJS += \
./cpi_code.o 

C_DEPS += \
./cpi_code.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"D:\project\cpi_test\libs\crypto" -I"D:\project\cpi_test\libs\libecclight\curves" -I"D:\project\cpi_test\libs\libecclight\external_deps" -I"D:\project\cpi_test\libs\libecclight\fp" -I"D:\project\cpi_test\libs\libecclight\nn" -I"D:\project\cpi_test\libs\libecclight\utils" -I"D:\project\cpi_test\libs\libecclight\words" -I"D:\project\cpi_test\libs\libecclight" -I"D:\project\cpi_test\libs\hmac" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


