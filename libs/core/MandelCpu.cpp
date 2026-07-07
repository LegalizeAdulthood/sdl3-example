#include <core/MandelCpu.h>

#include <cmath>

namespace core
{

int mandel_cpu_pixel(const MandelParams &params, int px, int py)
{
    const double cx = params.x_min + static_cast<double>(px) * params.dx;
    const double cy = params.y_min + static_cast<double>(py) * params.dy;

    double x = cx + params.z0_real;
    double y = cy + params.z0_imag;
    double saved_x = 0.0;
    double saved_y = 0.0;
    int saved_and = params.first_saved_and;
    int saved_incr = 1;

    for (int iter = 1; iter < params.max_iterations; ++iter)
    {
        const double x2 = x * x;
        const double y2 = y * y;
        const double xy = x * y;

        x = x2 - y2 + cx;
        y = 2.0 * xy + cy;

        if (x * x + y * y >= params.bailout)
        {
            return iter;
        }

        if (params.periodicity_check != 0)
        {
            if ((iter & saved_and) == 0)
            {
                saved_x = x;
                saved_y = y;
                --saved_incr;

                if (saved_incr == 0)
                {
                    saved_and = (saved_and << 1) + 1;
                    saved_incr = params.periodicity_next_saved_incr;
                }
            }
            else if (std::abs(saved_x - x) < params.close_enough && std::abs(saved_y - y) < params.close_enough)
            {
                return params.max_iterations;
            }
        }
    }

    return params.max_iterations;
}

} // namespace core
