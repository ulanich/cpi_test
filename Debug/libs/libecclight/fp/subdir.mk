################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../libs/libecclight/fp/fp.c \
../libs/libecclight/fp/fp_add.c \
../libs/libecclight/fp/fp_montgomery.c \
../libs/libecclight/fp/fp_mul.c \
../libs/libecclight/fp/fp_mul_redc1.c \
../libs/libecclight/fp/fp_rand.c 

OBJS += \
./libs/libecclight/fp/fp.o \
./libs/libecclight/fp/fp_add.o \
./libs/libecclight/fp/fp_montgomery.o \
./libs/libecclight/fp/fp_mul.o \
./libs/libecclight/fp/fp_mul_redc1.o \
./libs/libecclight/fp/fp_rand.o 

C_DEPS += \
./libs/libecclight/fp/fp.d \
./libs/libecclight/fp/fp_add.d \
./libs/libecclight/fp/fp_montgomery.d \
./libs/libecclight/fp/fp_mul.d \
./libs/libecclight/fp/fp_mul_redc1.d \
./libs/libecclight/fp/fp_rand.d 


# Each subdirectory must supply rules for building sources it contributes
libs/libecclight/fp/%.o: ../libs/libecclight/fp/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"D:\project\cpi_test\libs\crypto" -I"D:\project\cpi_test\libs\libecclight\curves" -I"D:\project\cpi_test\libs\libecclight\external_deps" -I"D:\project\cpi_test\libs\libecclight\fp" -I"D:\project\cpi_test\libs\libecclight\nn" -I"D:\project\cpi_test\libs\libecclight\utils" -I"D:\project\cpi_test\libs\libecclight\words" -I"D:\project\cpi_test\libs\libecclight" -I"D:\project\cpi_test\libs\hmac" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


