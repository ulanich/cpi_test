################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../libs/hmac/hmac_sha2.c \
../libs/hmac/sha2.c 

OBJS += \
./libs/hmac/hmac_sha2.o \
./libs/hmac/sha2.o 

C_DEPS += \
./libs/hmac/hmac_sha2.d \
./libs/hmac/sha2.d 


# Each subdirectory must supply rules for building sources it contributes
libs/hmac/%.o: ../libs/hmac/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"D:\project\cpi_code\libs\crypto" -I"D:\project\cpi_code\libs\libecclight\curves" -I"D:\project\cpi_code\libs\libecclight\external_deps" -I"D:\project\cpi_code\libs\libecclight\fp" -I"D:\project\cpi_code\libs\libecclight\nn" -I"D:\project\cpi_code\libs\libecclight\utils" -I"D:\project\cpi_code\libs\libecclight\words" -I"D:\project\cpi_code\libs\libecclight" -I"D:\project\cpi_code\libs\hmac" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


