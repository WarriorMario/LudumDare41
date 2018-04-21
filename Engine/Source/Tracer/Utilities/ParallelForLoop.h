#pragma once
#include <functional>
#include "Typedefs.h"

// the parallel for loop function. really fast stuff m8.
void ParallelFor(std::function<void(int32)> &func, int32 count, int32 chunkSize);

int32 NumSystemCores();