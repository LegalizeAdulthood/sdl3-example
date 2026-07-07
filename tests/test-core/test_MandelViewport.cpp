#include <core/MandelViewport.h>

#include <gtest/gtest.h>

namespace
{

double pixel_x(const core::MandelParams &params, int px)
{
    return params.x_min + static_cast<double>(px) * params.dx;
}

double pixel_y(const core::MandelParams &params, int py)
{
    return params.y_min + static_cast<double>(py) * params.dy;
}

} // namespace

TEST(MandelViewport, BuildsInitialParamsWithImageAspect)
{
    const core::MandelViewport viewport;

    const auto params = viewport.params(800, 600);

    EXPECT_DOUBLE_EQ(-2.5, params.x_min);
    EXPECT_DOUBLE_EQ(-1.5, params.y_min);
    EXPECT_DOUBLE_EQ(4.0 / 800.0, params.dx);
    EXPECT_DOUBLE_EQ(3.0 / 600.0, params.dy);
    EXPECT_EQ(800, params.width);
    EXPECT_EQ(600, params.height);
    EXPECT_EQ(1000, params.max_iterations);
}

TEST(MandelViewport, PansOppositeTheMouseDrag)
{
    core::MandelViewport viewport;

    viewport.pan_pixels(10, -5, 100, 50);

    const auto params = viewport.params(100, 50);
    EXPECT_DOUBLE_EQ(-2.9, params.x_min);
    EXPECT_DOUBLE_EQ(-0.8, params.y_min);
}

TEST(MandelViewport, ZoomKeepsAnchorPixelFixed)
{
    core::MandelViewport viewport;
    const auto before = viewport.params(100, 50);
    const int px = 25;
    const int py = 10;
    const double anchor_x = pixel_x(before, px);
    const double anchor_y = pixel_y(before, py);

    viewport.zoom_at(0.5, px, py, 100, 50);

    const auto after = viewport.params(100, 50);
    EXPECT_DOUBLE_EQ(before.dx * 0.5, after.dx);
    EXPECT_DOUBLE_EQ(anchor_x, pixel_x(after, px));
    EXPECT_DOUBLE_EQ(anchor_y, pixel_y(after, py));
}
