#include <core/MandelImage.h>

#include <gtest/gtest.h>

#include <sstream>

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

TEST(MandelPalette, ReadsRgbRowsAndIgnoresTrailingText)
{
    std::istringstream input{"48 49 50 SPDX-License-Identifier: GPL-3.0-only\n"
                             "1 2 3\n"
                             "not a color\n"};

    const auto palette = core::read_mandel_palette(input);

    ASSERT_EQ(2u, palette.colors.size());
    expect_color(palette.colors[0], 48, 49, 50);
    expect_color(palette.colors[1], 1, 2, 3);
}
