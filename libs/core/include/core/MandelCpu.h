#pragma once

#include <core/MandelIterationBuffer.h>
#include <core/MandelParams.h>

namespace core
{

int mandel_cpu_pixel(const MandelParams &params, int px, int py);
MandelIterationBuffer render_mandel_cpu(const MandelParams &params);

} // namespace core
