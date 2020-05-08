################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: GNU Compiler'
	"/Users/Tilman/Documents/CCS/ccsv8/tools/compiler/gcc-arm-none-eabi-7-2017-q4-major/bin/arm-none-eabi-gcc" -c -mcpu=cortex-m4 -march=armv7e-m -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DPART_TM4C123GH6PM -I"/Users/Tilman/Documents/Programme/C/SDU/embedded-programming-sdu/workspace/lab7_assig6_til" -I"/Users/Tilman/Documents/CCS/ccsv8/tools/compiler/gcc-arm-none-eabi-7-2017-q4-major/arm-none-eabi/include" -I"/Users/Tilman/Documents/Programme/C/SDU/embedded-programming-sdu/headers" -O0 -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -Wall -specs="nosys.specs" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)"  -v $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


