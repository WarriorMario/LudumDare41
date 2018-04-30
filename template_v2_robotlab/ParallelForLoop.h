#pragma once
#include <functional>

// the parallel for loop function. really fast stuff m8.
void ParallelFor(std::function<void(int)> &func, int count, int chunkSize);

int NumSystemCores();