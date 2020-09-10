################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../libs/libecclight/utils/print_fp.c \
../libs/libecclight/utils/print_nn.c \
../libs/libecclight/utils/utils.c 

OBJS += \
./libs/libecclight/utils/print_fp.o \
./libs/libecclight/utils/print_nn.o \
./libs/libecclight/utils/utils.o 

C_DEPS += \
./libs/libecclight/utils/print_fp.d \
./libs/libecclight/utils/print_nn.d \
./libs/libecclight/utils/utils.d 


# Each subdirectory must supply rules for building sources it contributes
libs/libecclight/utils/%.o: ../libs/libecclight/utils/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"D:\project\cpi_code\libs\crypto" -I"D:\project\cpi_code\libs\libecclight\curves" -I"D:\project\cpi_code\libs\libecclight\external_deps" -I"D:\project\cpi_code\libs\libecclight\fp" -I"D:\project\cpi_code\libs\libecclight\nn" -I"D:\project\cpi_code\libs\libecclight\utils" -I"D:\project\cpi_code\libs\libecclight\words" -I"D:\project\cpi_code\libs\libecclight" -I"D:\project\cpi_code\libs\hmac" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


