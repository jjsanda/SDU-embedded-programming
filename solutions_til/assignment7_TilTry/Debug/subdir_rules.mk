################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: GNU Compiler'
	"/Users/Tilman/Documents/CCS/ccsv8/tools/compiler/gcc-arm-none-eabi-7-2017-q4-major/bin/arm-none-eabi-gcc" -c -mcpu=cortex-m4 -march=armv7e-m -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DPART_TM4C123GH6PM -I"/Users/Tilman/Documents/Programme/C/SDU/embedded-programming-sdu/workspace/assignment7_TilTry" -I"/Users/Tilman/Documents/CCS/ccsv8/tools/compiler/gcc-arm-none-eabi-7-2017-q4-major/arm-none-eabi/include" -I"/Users/Tilman/Documents/Programme/C/SDU/embedded-programming-sdu/headers" -I"/Users/Tilman/Documents/Programme/C/SDU/embedded-programming-sdu/frt10/inc" -I"/Users/Tilman/Documents/Programme/C/SDU/embedded-programming-sdu/frt10/port/TivaM4" -O0 -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -Wall -specs="nosys.specs" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)"  -v $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

heap_2.o: /Users/Tilman/Documents/Programme/C/SDU/embedded-programming-sdu/frt10/port/MemMang/heap_2.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: GNU Compiler'
	"/Users/Tilman/Documents/CCS/ccsv8/tools/compiler/gcc-arm-none-eabi-7-2017-q4-major/bin/arm-none-eabi-gcc" -c -mcpu=cortex-m4 -march=armv7e-m -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DPART_TM4C123GH6PM -I"/Users/Tilman/Documents/Programme/C/SDU/embedded-programming-sdu/workspace/assignment7_TilTry" -I"/Users/Tilman/Documents/CCS/ccsv8/tools/compiler/gcc-arm-none-eabi-7-2017-q4-major/arm-none-eabi/include" -I"/Users/Tilman/Documents/Programme/C/SDU/embedded-programming-sdu/headers" -I"/Users/Tilman/Documents/Programme/C/SDU/embedded-programming-sdu/frt10/inc" -I"/Users/Tilman/Documents/Programme/C/SDU/embedded-programming-sdu/frt10/port/TivaM4" -O0 -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -Wall -specs="nosys.specs" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)"  -v $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

list.o: /Users/Tilman/Documents/Programme/C/SDU/embedded-programming-sdu/frt10/src/list.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: GNU Compiler'
	"/Users/Tilman/Documents/CCS/ccsv8/tools/compiler/gcc-arm-none-eabi-7-2017-q4-major/bin/arm-none-eabi-gcc" -c -mcpu=cortex-m4 -march=armv7e-m -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DPART_TM4C123GH6PM -I"/Users/Tilman/Documents/Programme/C/SDU/embedded-programming-sdu/workspace/assignment7_TilTry" -I"/Users/Tilman/Documents/CCS/ccsv8/tools/compiler/gcc-arm-none-eabi-7-2017-q4-major/arm-none-eabi/include" -I"/Users/Tilman/Documents/Programme/C/SDU/embedded-programming-sdu/headers" -I"/Users/Tilman/Documents/Programme/C/SDU/embedded-programming-sdu/frt10/inc" -I"/Users/Tilman/Documents/Programme/C/SDU/embedded-programming-sdu/frt10/port/TivaM4" -O0 -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -Wall -specs="nosys.specs" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)"  -v $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

port.o: /Users/Tilman/Documents/Programme/C/SDU/embedded-programming-sdu/frt10/port/TivaM4/port.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: GNU Compiler'
	"/Users/Tilman/Documents/CCS/ccsv8/tools/compiler/gcc-arm-none-eabi-7-2017-q4-major/bin/arm-none-eabi-gcc" -c -mcpu=cortex-m4 -march=armv7e-m -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DPART_TM4C123GH6PM -I"/Users/Tilman/Documents/Programme/C/SDU/embedded-programming-sdu/workspace/assignment7_TilTry" -I"/Users/Tilman/Documents/CCS/ccsv8/tools/compiler/gcc-arm-none-eabi-7-2017-q4-major/arm-none-eabi/include" -I"/Users/Tilman/Documents/Programme/C/SDU/embedded-programming-sdu/headers" -I"/Users/Tilman/Documents/Programme/C/SDU/embedded-programming-sdu/frt10/inc" -I"/Users/Tilman/Documents/Programme/C/SDU/embedded-programming-sdu/frt10/port/TivaM4" -O0 -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -Wall -specs="nosys.specs" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)"  -v $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

queue.o: /Users/Tilman/Documents/Programme/C/SDU/embedded-programming-sdu/frt10/src/queue.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: GNU Compiler'
	"/Users/Tilman/Documents/CCS/ccsv8/tools/compiler/gcc-arm-none-eabi-7-2017-q4-major/bin/arm-none-eabi-gcc" -c -mcpu=cortex-m4 -march=armv7e-m -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DPART_TM4C123GH6PM -I"/Users/Tilman/Documents/Programme/C/SDU/embedded-programming-sdu/workspace/assignment7_TilTry" -I"/Users/Tilman/Documents/CCS/ccsv8/tools/compiler/gcc-arm-none-eabi-7-2017-q4-major/arm-none-eabi/include" -I"/Users/Tilman/Documents/Programme/C/SDU/embedded-programming-sdu/headers" -I"/Users/Tilman/Documents/Programme/C/SDU/embedded-programming-sdu/frt10/inc" -I"/Users/Tilman/Documents/Programme/C/SDU/embedded-programming-sdu/frt10/port/TivaM4" -O0 -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -Wall -specs="nosys.specs" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)"  -v $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

startup_frt.o: /Users/Tilman/Documents/Programme/C/SDU/embedded-programming-sdu/frt10/port/TivaM4/startup_frt.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: GNU Compiler'
	"/Users/Tilman/Documents/CCS/ccsv8/tools/compiler/gcc-arm-none-eabi-7-2017-q4-major/bin/arm-none-eabi-gcc" -c -mcpu=cortex-m4 -march=armv7e-m -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DPART_TM4C123GH6PM -I"/Users/Tilman/Documents/Programme/C/SDU/embedded-programming-sdu/workspace/assignment7_TilTry" -I"/Users/Tilman/Documents/CCS/ccsv8/tools/compiler/gcc-arm-none-eabi-7-2017-q4-major/arm-none-eabi/include" -I"/Users/Tilman/Documents/Programme/C/SDU/embedded-programming-sdu/headers" -I"/Users/Tilman/Documents/Programme/C/SDU/embedded-programming-sdu/frt10/inc" -I"/Users/Tilman/Documents/Programme/C/SDU/embedded-programming-sdu/frt10/port/TivaM4" -O0 -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -Wall -specs="nosys.specs" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)"  -v $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

systick_frt.o: /Users/Tilman/Documents/Programme/C/SDU/embedded-programming-sdu/frt10/port/TivaM4/systick_frt.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: GNU Compiler'
	"/Users/Tilman/Documents/CCS/ccsv8/tools/compiler/gcc-arm-none-eabi-7-2017-q4-major/bin/arm-none-eabi-gcc" -c -mcpu=cortex-m4 -march=armv7e-m -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DPART_TM4C123GH6PM -I"/Users/Tilman/Documents/Programme/C/SDU/embedded-programming-sdu/workspace/assignment7_TilTry" -I"/Users/Tilman/Documents/CCS/ccsv8/tools/compiler/gcc-arm-none-eabi-7-2017-q4-major/arm-none-eabi/include" -I"/Users/Tilman/Documents/Programme/C/SDU/embedded-programming-sdu/headers" -I"/Users/Tilman/Documents/Programme/C/SDU/embedded-programming-sdu/frt10/inc" -I"/Users/Tilman/Documents/Programme/C/SDU/embedded-programming-sdu/frt10/port/TivaM4" -O0 -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -Wall -specs="nosys.specs" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)"  -v $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

tasks.o: /Users/Tilman/Documents/Programme/C/SDU/embedded-programming-sdu/frt10/src/tasks.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: GNU Compiler'
	"/Users/Tilman/Documents/CCS/ccsv8/tools/compiler/gcc-arm-none-eabi-7-2017-q4-major/bin/arm-none-eabi-gcc" -c -mcpu=cortex-m4 -march=armv7e-m -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DPART_TM4C123GH6PM -I"/Users/Tilman/Documents/Programme/C/SDU/embedded-programming-sdu/workspace/assignment7_TilTry" -I"/Users/Tilman/Documents/CCS/ccsv8/tools/compiler/gcc-arm-none-eabi-7-2017-q4-major/arm-none-eabi/include" -I"/Users/Tilman/Documents/Programme/C/SDU/embedded-programming-sdu/headers" -I"/Users/Tilman/Documents/Programme/C/SDU/embedded-programming-sdu/frt10/inc" -I"/Users/Tilman/Documents/Programme/C/SDU/embedded-programming-sdu/frt10/port/TivaM4" -O0 -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -Wall -specs="nosys.specs" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)"  -v $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


