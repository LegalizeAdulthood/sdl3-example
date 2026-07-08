#include <core/MandelImage.h>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <istream>
#include <sstream>
#include <string>

namespace core
{

namespace
{

std::uint8_t to_byte(int value)
{
    return static_cast<std::uint8_t>(std::clamp(value, 0, 255));
}

std::uint8_t blend_channel(std::uint8_t first, std::uint8_t second, float blend)
{
    const float value = static_cast<float>(first) + (static_cast<float>(second) - static_cast<float>(first)) * blend;
    return to_byte(static_cast<int>(std::lround(value)));
}

Rgba8 blend_color(const Rgba8 &first, const Rgba8 &second, float blend)
{
    return Rgba8{blend_channel(first.red, second.red, blend), blend_channel(first.green, second.green, blend),
        blend_channel(first.blue, second.blue, blend), blend_channel(first.alpha, second.alpha, blend)};
}

} // namespace

MandelPalette read_mandel_palette(std::istream &input)
{
    MandelPalette palette;
    std::string line;

    while (std::getline(input, line))
    {
        std::istringstream fields(line);
        int red = 0;
        int green = 0;
        int blue = 0;
        if (fields >> red >> green >> blue)
        {
            palette.colors.push_back(Rgba8{to_byte(red), to_byte(green), to_byte(blue), 255});
        }
    }

    return palette;
}

Rgba8 mandel_color(float iteration, int max_iterations)
{
    if (max_iterations <= 0 || iteration >= max_iterations)
    {
        return Rgba8{0, 0, 0, 255};
    }

    const float clamped = std::clamp(iteration, 0.0f, static_cast<float>(max_iterations));
    const auto shade = static_cast<std::uint8_t>(std::lround(255.0f * clamped / static_cast<float>(max_iterations)));
    return Rgba8{shade, shade, shade, 255};
}

Rgba8 mandel_color(float iteration, int max_iterations, const MandelPalette &palette)
{
    if (max_iterations <= 0 || iteration >= max_iterations || palette.colors.empty())
    {
        return Rgba8{0, 0, 0, 255};
    }

    const float clamped = std::max(iteration, 0.0f);
    const float wrapped = std::fmod(clamped, static_cast<float>(palette.colors.size()));
    const float base = std::floor(wrapped);
    const auto first_index = static_cast<std::size_t>(base);
    const auto second_index = (first_index + 1) % palette.colors.size();
    return blend_color(palette.colors[first_index], palette.colors[second_index], wrapped - base);
}

MandelImage map_mandel_colors(const MandelIterationBuffer &buffer, int max_iterations)
{
    MandelImage image{buffer.width, buffer.height, std::vector<Rgba8>(buffer.iterations.size())};

    std::transform(buffer.iterations.begin(), buffer.iterations.end(), image.pixels.begin(),
        [max_iterations](float iteration) { return mandel_color(iteration, max_iterations); });

    return image;
}

MandelImage map_mandel_colors(const MandelIterationBuffer &buffer, int max_iterations, const MandelPalette &palette)
{
    MandelImage image{buffer.width, buffer.height, std::vector<Rgba8>(buffer.iterations.size())};

    std::transform(buffer.iterations.begin(), buffer.iterations.end(), image.pixels.begin(),
        [max_iterations, &palette](float iteration) { return mandel_color(iteration, max_iterations, palette); });

    return image;
}

} // namespace core
