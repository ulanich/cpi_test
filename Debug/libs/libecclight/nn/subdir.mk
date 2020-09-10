################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../libs/libecclight/nn/nn.c \
../libs/libecclight/nn/nn_add.c \
../libs/libecclight/nn/nn_div.c \
../libs/libecclight/nn/nn_logical.c \
../libs/libecclight/nn/nn_modinv.c \
../libs/libecclight/nn/nn_mul.c \
../libs/libecclight/nn/nn_mul_redc1.c \
../libs/libecclight/nn/nn_rand.c 

OBJS += \
./libs/libecclight/nn/nn.o \
./libs/libecclight/nn/nn_add.o \
./libs/libecclight/nn/nn_div.o \
./libs/libecclight/nn/nn_logical.o \
./libs/libecclight/nn/nn_modinv.o \
./libs/libecclight/nn/nn_mul.o \
./libs/libecclight/nn/nn_mul_redc1.o \
./libs/libecclight/nn/nn_rand.o 

C_DEPS += \
./libs/libecclight/nn/nn.d \
./libs/libecclight/nn/nn_add.d \
./libs/libecclight/nn/nn_div.d \
./libs/libecclight/nn/nn_logical.d \
./libs/libecclight/nn/nn_modinv.d \
./libs/libecclight/nn/nn_mul.d \
./libs/libecclight/nn/nn_mul_redc1.d \
./libs/libecclight/nn/nn_rand.d 


# Each subdirectory must supply rules for building sources it contributes
libs/libecclight/nn/%.o: ../libs/libecclight/nn/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"D:\project\cpi_test\libs\crypto" -I"D:\project\cpi_test\libs\libecclight\curves" -I"D:\project\cpi_test\libs\libecclight\external_deps" -I"D:\project\cpi_test\libs\libecclight\fp" -I"D:\project\cpi_test\libs\libecclight\nn" -I"D:\project\cpi_test\libs\libecclight\utils" -I"D:\project\cpi_test\libs\libecclight\words" -I"D:\project\cpi_test\libs\libecclight" -I"D:\project\cpi_test\libs\hmac" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


