################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/debug.c 

CC_SRCS += \
../src/TTS.cc \
../src/TextObjectTTS.cc \
../src/UnitSelector.cc \
../src/TTS_test.cc \
../src/Sound.cc

OBJS += \
./src/TTS.o \
./src/TextObjectTTS.o \
./src/UnitSelector.o \
./src/debug.o \
./src/TTS_test.o \
./src/Sound.o

C_DEPS += \
./src/debug.d 

CC_DEPS += \
./src/TTS.d \
./src/TextObjectTTS.d \
./src/UnitSelector.d \
./src/TTS_test.d \
./src/Sound.d

# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -I$(TOP_DIR)/include -I$(TOP_DIR)/unittest -I/usr/lib/arm-linux-gnueabihf/glib-2.0/include -I/usr/include/glib-2.0 -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -I$(TOP_DIR)/include -I$(TOP_DIR)/unittest -I/usr/lib/arm-linux-gnueabihf/glib-2.0/include -I/usr/include/glib-2.0 -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


