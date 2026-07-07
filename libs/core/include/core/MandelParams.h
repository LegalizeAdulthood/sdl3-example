#pragma once

namespace core
{

struct MandelParams
{
    double x_min;
    double y_min;
    double dx;
    double dy;
    double z0_real;
    double z0_imag;
    double bailout;
    double close_enough;
    int width;
    int height;
    int max_iterations;
    int first_saved_and;
    int periodicity_next_saved_incr;
    int periodicity_check;
};

} // namespace core
