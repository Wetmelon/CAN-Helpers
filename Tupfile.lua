-- Compile test_asm.cpp to assembly
tup.rule('Test/test_asm.cpp',
    '^j^ arm-none-eabi-g++ -c -o %o %f -O3 -std=c++17 -I. -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard',
    'Test/test_asm.o')

-- Dump assembly output
tup.rule('Test/test_asm.o',
    'arm-none-eabi-objdump -dSC %f > %o',
    'Test/test_asm.s')
