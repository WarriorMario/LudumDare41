#pragma once
#include <cstdint>

class Timer
{
public:
	Timer();
	void Reset();
	double Elapsed();

private:
	int64_t cyclesPassed;
	double frequency;
};