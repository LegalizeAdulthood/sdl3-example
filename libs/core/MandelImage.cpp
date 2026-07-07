#include <core/MandelImage.h>

#include <algorithm>
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

Rgba8 mandel_color(int iteration, int max_iterations)
{
    if (max_iterations <= 0 || iteration >= max_iterations)
    {
        return Rgba8{0, 0, 0, 255};
    }

    const int clamped = std::clamp(iteration, 0, max_iterations);
    const auto shade = static_cast<std::uint8_t>(255 * clamped / max_iterations);
    return Rgba8{shade, shade, shade, 255};
}

Rgba8 mandel_color(int iteration, int max_iterations, const MandelPalette &palette)
{
    if (max_iterations <= 0 || iteration >= max_iterations || palette.colors.empty())
    {
        return Rgba8{0, 0, 0, 255};
    }

    const int clamped = std::max(iteration, 0);
    const auto index = static_cast<std::size_t>(clamped) % palette.colors.size();
    return palette.colors[index];
}

MandelImage map_mandel_colors(const MandelIterationBuffer &buffer, int max_iterations)
{
    MandelImage image{buffer.width, buffer.height, std::vector<Rgba8>(buffer.iterations.size())};

    std::transform(buffer.iterations.begin(), buffer.iterations.end(), image.pixels.begin(),
        [max_iterations](int iteration) { return mandel_color(iteration, max_iterations); });

    return image;
}

MandelImage map_mandel_colors(const MandelIterationBuffer &buffer, int max_iterations, const MandelPalette &palette)
{
    MandelImage image{buffer.width, buffer.height, std::vector<Rgba8>(buffer.iterations.size())};

    std::transform(buffer.iterations.begin(), buffer.iterations.end(), image.pixels.begin(),
        [max_iterations, &palette](int iteration) { return mandel_color(iteration, max_iterations, palette); });

    return image;
}

} // namespace core
