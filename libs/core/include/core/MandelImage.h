#pragma once

#include <core/MandelIterationBuffer.h>

#include <cstdint>
#include <iosfwd>
#include <vector>

namespace core
{

struct Rgba8
{
    std::uint8_t red = 0;
    std::uint8_t green = 0;
    std::uint8_t blue = 0;
    std::uint8_t alpha = 255;
};

struct MandelImage
{
    int width = 0;
    int height = 0;
    std::vector<Rgba8> pixels;
};

struct MandelPalette
{
    std::vector<Rgba8> colors;
};

MandelPalette read_mandel_palette(std::istream &input);
Rgba8 mandel_color(float iteration, int max_iterations);
Rgba8 mandel_color(float iteration, int max_iterations, const MandelPalette &palette);
MandelImage map_mandel_colors(const MandelIterationBuffer &buffer, int max_iterations);
MandelImage map_mandel_colors(const MandelIterationBuffer &buffer, int max_iterations, const MandelPalette &palette);

} // namespace core
