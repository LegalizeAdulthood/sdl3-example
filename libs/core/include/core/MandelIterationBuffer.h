#pragma once

#include <vector>

namespace core
{

struct MandelIterationBuffer
{
    int width = 0;
    int height = 0;
    std::vector<float> iterations;
};

} // namespace core
