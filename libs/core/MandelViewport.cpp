#include <core/MandelViewport.h>

namespace core
{

namespace
{

double view_height(double view_width, int width, int height)
{
    return view_width * static_cast<double>(height) / static_cast<double>(width);
}

} // namespace

MandelParams MandelViewport::params(int width, int height) const
{
    const double height_world = view_height(m_viewWidth, width, height);

    return MandelParams{m_centerX - m_viewWidth / 2.0, m_centerY - height_world / 2.0,
        m_viewWidth / static_cast<double>(width), height_world / static_cast<double>(height), 0.0, 0.0, 4.0, 1.0e-7,
        width, height, 1000, 15, 10, 1};
}

void MandelViewport::pan_pixels(int delta_x, int delta_y, int width, int height)
{
    if (width <= 0 || height <= 0)
    {
        return;
    }

    const auto before = params(width, height);
    m_centerX -= static_cast<double>(delta_x) * before.dx;
    m_centerY -= static_cast<double>(delta_y) * before.dy;
}

void MandelViewport::zoom_at(double scale, int px, int py, int width, int height)
{
    if (width <= 0 || height <= 0 || scale <= 0.0)
    {
        return;
    }

    const auto before = params(width, height);
    const double anchor_x = before.x_min + static_cast<double>(px) * before.dx;
    const double anchor_y = before.y_min + static_cast<double>(py) * before.dy;

    m_viewWidth *= scale;
    const double height_world = view_height(m_viewWidth, width, height);
    const double dx = m_viewWidth / static_cast<double>(width);
    const double dy = height_world / static_cast<double>(height);

    m_centerX = anchor_x - static_cast<double>(px) * dx + m_viewWidth / 2.0;
    m_centerY = anchor_y - static_cast<double>(py) * dy + height_world / 2.0;
}

} // namespace core
