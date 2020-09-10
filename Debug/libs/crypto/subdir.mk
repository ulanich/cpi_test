################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../libs/crypto/aes-gcm.c \
../libs/crypto/aes-internal-dec.c \
../libs/crypto/aes-internal-enc.c \
../libs/crypto/aes-internal.c \
../libs/crypto/aes_ecb.c 

OBJS += \
./libs/crypto/aes-gcm.o \
./libs/crypto/aes-internal-dec.o \
./libs/crypto/aes-internal-enc.o \
./libs/crypto/aes-internal.o \
./libs/crypto/aes_ecb.o 

C_DEPS += \
./libs/crypto/aes-gcm.d \
./libs/crypto/aes-internal-dec.d \
./libs/crypto/aes-internal-enc.d \
./libs/crypto/aes-internal.d \
./libs/crypto/aes_ecb.d 


# Each subdirectory must supply rules for building sources it contributes
libs/crypto/%.o: ../libs/crypto/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"D:\project\cpi_code\libs\crypto" -I"D:\project\cpi_code\libs\libecclight\curves" -I"D:\project\cpi_code\libs\libecclight\external_deps" -I"D:\project\cpi_code\libs\libecclight\fp" -I"D:\project\cpi_code\libs\libecclight\nn" -I"D:\project\cpi_code\libs\libecclight\utils" -I"D:\project\cpi_code\libs\libecclight\words" -I"D:\project\cpi_code\libs\libecclight" -I"D:\project\cpi_code\libs\hmac" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


