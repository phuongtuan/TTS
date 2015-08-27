################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../unittest/testUnitSelector.cc 

OBJS += \
./unittest/testUnitSelector.o 

CC_DEPS += \
./unittest/testUnitSelector.d 

# Each subdirectory must supply rules for building sources it contributes
unittest/%.o: ../unittest/%.cc
	@echo '  CXX        $<'	
	@g++ -I$(TOP_DIR)/include -I$(TOP_DIR)/unittest -I$(TOP_DIR)/include-I/usr/lib/arm-linux-gnueabihf/glib-2.0/include -I/usr/include/glib-2.0 -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	