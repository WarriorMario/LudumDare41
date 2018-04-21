#include "utilities\Timer.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

Timer::Timer()
{
	Reset();
}

void Timer::Reset()
{
	LARGE_INTEGER integer;

	QueryPerformanceFrequency(&integer);
	frequency = double(integer.QuadPart) / 1000.0;


	QueryPerformanceCounter(&integer);
	cyclesPassed = integer.QuadPart;
}

double Timer::Elapsed()
{
	LARGE_INTEGER curPassed;
	QueryPerformanceCounter(&curPassed);
	return double(curPassed.QuadPart - cyclesPassed) / frequency;
}
