################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../libs/libecclight/curves/aff_pt.c \
../libs/libecclight/curves/curves.c \
../libs/libecclight/curves/ec_params.c \
../libs/libecclight/curves/ec_shortw.c \
../libs/libecclight/curves/prj_pt.c \
../libs/libecclight/curves/prj_pt_monty.c 

OBJS += \
./libs/libecclight/curves/aff_pt.o \
./libs/libecclight/curves/curves.o \
./libs/libecclight/curves/ec_params.o \
./libs/libecclight/curves/ec_shortw.o \
./libs/libecclight/curves/prj_pt.o \
./libs/libecclight/curves/prj_pt_monty.o 

C_DEPS += \
./libs/libecclight/curves/aff_pt.d \
./libs/libecclight/curves/curves.d \
./libs/libecclight/curves/ec_params.d \
./libs/libecclight/curves/ec_shortw.d \
./libs/libecclight/curves/prj_pt.d \
./libs/libecclight/curves/prj_pt_monty.d 


# Each subdirectory must supply rules for building sources it contributes
libs/libecclight/curves/%.o: ../libs/libecclight/curves/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"D:\project\cpi_code\libs\crypto" -I"D:\project\cpi_code\libs\libecclight\curves" -I"D:\project\cpi_code\libs\libecclight\external_deps" -I"D:\project\cpi_code\libs\libecclight\fp" -I"D:\project\cpi_code\libs\libecclight\nn" -I"D:\project\cpi_code\libs\libecclight\utils" -I"D:\project\cpi_code\libs\libecclight\words" -I"D:\project\cpi_code\libs\libecclight" -I"D:\project\cpi_code\libs\hmac" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


