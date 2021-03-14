
all:
	@g++ -O3 -std=c++17 Test/test_can.cpp -o Test/test_runner.exe
	@Test/test_runner.exe