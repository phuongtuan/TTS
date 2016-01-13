################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/debug.c 


CC_SRCS += \
../src/HtmlParser.cc \
../src/NewsReader.cc \
../src/Sound.cc \
../src/TTS.cc \
../src/TextObjectTTS.cc \
../src/UnitSelector.cc \
../src/get_html_link.cc 

OBJS += \
./src/HtmlParser.o \
./src/NewsReader.o \
./src/Sound.o \
./src/TTS.o \
./src/TextObjectTTS.o \
./src/UnitSelector.o \
./src/debug.o \
./src/get_html_link.o 

C_DEPS += \
./src/debug.d 


CC_DEPS += \
./src/HtmlParser.d \
./src/NewsReader.d \
./src/Sound.d \
./src/TTS.d \
./src/TextObjectTTS.d \
./src/UnitSelector.d \
./src/get_html_link.d 

# Each subdirectory must supply rules for building sources it contributes	
src/%.o: ../src/%.cc
	@echo '  CXX        $<'	
	@g++ -I$(TOP_DIR)/include -I$(TOP_DIR)/unittest -I/usr/lib/arm-linux-gnueabihf/glib-2.0/include -I/usr/include/glib-2.0 -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	
src/%.o: ../src/%.c
	@echo '  CC         $<'	
	@gcc -I$(TOP_DIR)/include -I$(TOP_DIR)/unittest -I/usr/lib/arm-linux-gnueabihf/glib-2.0/include -I/usr/include/glib-2.0 -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	