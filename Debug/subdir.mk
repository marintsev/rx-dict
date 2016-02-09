################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../commands.c \
../db.c \
../entry.c \
../log.c \
../main.c \
../utils.c 

OBJS += \
./commands.o \
./db.o \
./entry.o \
./log.o \
./main.o \
./utils.o 

C_DEPS += \
./commands.d \
./db.d \
./entry.d \
./log.d \
./main.d \
./utils.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


