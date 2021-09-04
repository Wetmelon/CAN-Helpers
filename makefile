
all:
	@g++ -Ofast -Wall -Wextra -pedantic -std=gnu++14 Test/test_can.cpp -o Test/test_runner.exe
	@./Test/test_runner.exe