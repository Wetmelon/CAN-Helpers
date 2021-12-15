
all:
	@g++ -Ofast -Wall -Wextra -pedantic -std=gnu++11 Test/test_can.cpp -o Test/test_runner.exe
	@./Test/test_runner.exe

	@"C:/Program Files (x86)/Arduino/hardware/tools/arm/bin/arm-none-eabi-gcc" -std=gnu++11 -c can_helpers.hpp -o can_helpers_teensy.o -O2 -g -Wall -ffunction-sections -fdata-sections -nostdlib -MMD -mthumb -mcpu=cortex-m7 -mfloat-abi=hard -mfpu=fpv5-d16