#include <core/MandelCpu.h>

#include <gtest/gtest.h>

namespace
{

core::MandelParams two_point_params()
{
    return core::MandelParams{
        0.0,    // x_min
        0.0,    // y_min
        2.0,    // dx
        0.0,    // dy
        0.0,    // z0_real
        0.0,    // z0_imag
        4.0,    // bailout
        1.0e-7, // close_enough
        2,      // width
        1,      // height
        1000,   // max_iterations
        15,     // first_saved_and
        10,     // periodicity_next_saved_incr
        1       // periodicity_check
    };
}

} // namespace

TEST(MandelIterationBuffer, HasImageDimensions)
{
    const auto params = two_point_params();
    const auto buffer = core::render_mandel_cpu(params);

    EXPECT_EQ(params.width, buffer.width);
    EXPECT_EQ(params.height, buffer.height);
    EXPECT_EQ(2u, buffer.iterations.size());
}

TEST(MandelIterationBuffer, StoresSelectedIterationCounts)
{
    const auto params = two_point_params();
    const auto buffer = core::render_mandel_cpu(params);

    EXPECT_FLOAT_EQ(static_cast<float>(params.max_iterations), buffer.iterations[0]);
    EXPECT_GT(buffer.iterations[1], 1.0f);
    EXPECT_LT(buffer.iterations[1], 2.0f);
}
