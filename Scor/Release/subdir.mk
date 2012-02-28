################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Communication.cpp \
../Communication_Thread.cpp \
../FieldAnalyse.cpp \
../IAModule_Thread.cpp \
../Ordre_Thread.cpp \
../Robot_Thread.cpp \
../ThreadAnalyse.cpp \
../main.cpp 

OBJS += \
./Communication.o \
./Communication_Thread.o \
./FieldAnalyse.o \
./IAModule_Thread.o \
./Ordre_Thread.o \
./Robot_Thread.o \
./ThreadAnalyse.o \
./main.o 

CPP_DEPS += \
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
	g++ -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


