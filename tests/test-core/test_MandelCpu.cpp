#include <core/MandelCpu.h>

#include <gtest/gtest.h>

namespace
{

core::MandelParams point_params(double real, double imag)
{
    return core::MandelParams{
        real,   // x_min
        imag,   // y_min
        0.0,    // dx
        0.0,    // dy
        0.0,    // z0_real
        0.0,    // z0_imag
        4.0,    // bailout
        1.0e-7, // close_enough
        1,      // width
        1,      // height
        1000,   // max_iterations
        15,     // first_saved_and
        10,     // periodicity_next_saved_incr
        1       // periodicity_check
    };
}

} // namespace

TEST(MandelCpu, OriginIsInside)
{
    const auto params = point_params(0.0, 0.0);

    EXPECT_EQ(params.max_iterations, core::mandel_cpu_pixel(params, 0, 0));
}

TEST(MandelCpu, FarPointEscapesQuickly)
{
    const auto params = point_params(2.0, 2.0);

    EXPECT_EQ(1, core::mandel_cpu_pixel(params, 0, 0));
}

TEST(MandelCpu, PeriodicityOnAndOffStayBounded)
{
    auto params = point_params(0.0, 0.0);
    const int with_periodicity = core::mandel_cpu_pixel(params, 0, 0);

    params.periodicity_check = 0;
    const int without_periodicity = core::mandel_cpu_pixel(params, 0, 0);

    EXPECT_EQ(params.max_iterations, with_periodicity);
    EXPECT_EQ(params.max_iterations, without_periodicity);
}
