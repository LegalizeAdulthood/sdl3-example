#include <core/MandelImage.h>

#include <algorithm>

namespace core
{

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

MandelImage map_mandel_colors(const MandelIterationBuffer &buffer, int max_iterations)
{
    MandelImage image{buffer.width, buffer.height, std::vector<Rgba8>(buffer.iterations.size())};

    std::transform(buffer.iterations.begin(), buffer.iterations.end(), image.pixels.begin(),
        [max_iterations](int iteration) { return mandel_color(iteration, max_iterations); });

    return image;
}

} // namespace core
