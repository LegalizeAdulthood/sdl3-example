#include <core/MandelImage.h>

#include <gtest/gtest.h>

namespace
{

void expect_color(const core::Rgba8 &color, int red, int green, int blue)
{
    EXPECT_EQ(red, color.red);
    EXPECT_EQ(green, color.green);
    EXPECT_EQ(blue, color.blue);
    EXPECT_EQ(255, color.alpha);
}

} // namespace

TEST(MandelImage, HasIterationBufferDimensions)
{
    const core::MandelIterationBuffer buffer{3, 1, {1, 2, 4}};

    const auto image = core::map_mandel_colors(buffer, 4);

    EXPECT_EQ(buffer.width, image.width);
    EXPECT_EQ(buffer.height, image.height);
    EXPECT_EQ(3u, image.pixels.size());
}

TEST(MandelImage, MapsSelectedIterationCountsToColors)
{
    const core::MandelIterationBuffer buffer{3, 1, {1, 2, 4}};

    const auto image = core::map_mandel_colors(buffer, 4);

    expect_color(image.pixels[0], 63, 63, 63);
    expect_color(image.pixels[1], 127, 127, 127);
    expect_color(image.pixels[2], 0, 0, 0);
}
