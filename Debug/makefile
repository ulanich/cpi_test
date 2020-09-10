################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include libs/libecclight/utils/subdir.mk
-include libs/libecclight/nn/subdir.mk
-include libs/libecclight/fp/subdir.mk
-include libs/libecclight/external_deps/subdir.mk
-include libs/libecclight/curves/subdir.mk
-include libs/libecclight/subdir.mk
-include libs/hmac/subdir.mk
-include libs/crypto/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: cpi_code.exe

# Tool invocations
cpi_code.exe: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MinGW C Linker'
	gcc -L"D:\project\cpi_code\libs" -o "cpi_code.exe" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) cpi_code.exe
	-@echo ' '

.PHONY: all clean dependents

-include ../makefile.targets
