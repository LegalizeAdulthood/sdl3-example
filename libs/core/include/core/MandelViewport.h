#pragma once

#include <core/MandelParams.h>

namespace core
{

class MandelViewport
{
public:
    [[nodiscard]] MandelParams params(int width, int height) const;
    void pan_pixels(int delta_x, int delta_y, int width, int height);
    void zoom_at(double scale, int px, int py, int width, int height);

private:
    double m_centerX = -0.5;
    double m_centerY = 0.0;
    double m_viewWidth = 4.0;
};

} // namespace core
