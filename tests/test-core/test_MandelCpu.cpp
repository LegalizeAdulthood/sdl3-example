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
    const core::MandelParams params = point_params(0.0, 0.0);

    EXPECT_FLOAT_EQ(static_cast<float>(params.max_iterations), core::mandel_cpu_pixel(params, 0, 0));
}

TEST(MandelCpu, FarPointEscapesQuickly)
{
    const core::MandelParams params = point_params(2.0, 2.0);

    const float iteration = core::mandel_cpu_pixel(params, 0, 0);

    EXPECT_GT(iteration, 0.0f);
    EXPECT_LT(iteration, 1.0f);
}

TEST(MandelCpu, PeriodicityOnAndOffStayBounded)
{
    core::MandelParams params = point_params(0.0, 0.0);
    const float with_periodicity = core::mandel_cpu_pixel(params, 0, 0);

    params.periodicity_check = 0;
    const float without_periodicity = core::mandel_cpu_pixel(params, 0, 0);

    EXPECT_FLOAT_EQ(static_cast<float>(params.max_iterations), with_periodicity);
    EXPECT_FLOAT_EQ(static_cast<float>(params.max_iterations), without_periodicity);
}
