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
    const core::MandelIterationBuffer buffer{3, 1, {1.0f, 2.0f, 4.0f}};

    const core::MandelImage image = core::map_mandel_colors(buffer, 4);

    EXPECT_EQ(buffer.width, image.width);
    EXPECT_EQ(buffer.height, image.height);
    EXPECT_EQ(3u, image.pixels.size());
}

TEST(MandelImage, MapsSelectedIterationCountsToColors)
{
    const core::MandelIterationBuffer buffer{5, 1, {0.0f, 0.5f, 1.0f, 2.0f, 4.0f}};
    const core::MandelPalette palette{{{10, 20, 30, 255}, {40, 50, 60, 255}}};

    const core::MandelImage image = core::map_mandel_colors(buffer, 4, palette);

    expect_color(image.pixels[0], 10, 20, 30);
    expect_color(image.pixels[1], 25, 35, 45);
    expect_color(image.pixels[2], 40, 50, 60);
    expect_color(image.pixels[3], 10, 20, 30);
    expect_color(image.pixels[4], 0, 0, 0);
}
