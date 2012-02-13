################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../ComKh.cpp \
../Communication.cpp \
../Communication_Thread.cpp \
../FieldAnalyse.cpp \
../IAModule_Thread.cpp \
../Ordre_Thread.cpp \
../Robot_Thread.cpp \
../ThreadAnalyse.cpp \
../main.cpp 

OBJS += \
./ComKh.o \
./Communication.o \
./Communication_Thread.o \
./FieldAnalyse.o \
./IAModule_Thread.o \
./Ordre_Thread.o \
./Robot_Thread.o \
./ThreadAnalyse.o \
./main.o 

CPP_DEPS += \
./ComKh.d \
./Communication.d \
./Communication_Thread.d \
./FieldAnalyse.d \
./IAModule_Thread.d \
./Ordre_Thread.d \
./Robot_Thread.d \
./ThreadAnalyse.d \
./main.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/local/include/opencv -I/usr/local/include/opencv2 -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


