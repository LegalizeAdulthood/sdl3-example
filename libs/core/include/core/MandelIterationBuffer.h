#pragma once

#include <vector>

namespace core
{

struct MandelIterationBuffer
{
    int width = 0;
    int height = 0;
    std::vector<int> iterations;
};

} // namespace core
