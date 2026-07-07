#include "MandelRenderHost.h"

#include "assets.h"

#include <core/MandelCpu.h>
#include <core/MandelImage.h>

#include <wx/dcclient.h>
#include <wx/image.h>

#include <cstddef>
#include <fstream>
#include <stdexcept>
#include <string>

namespace mandel
{

namespace
{

core::MandelParams make_params(int width, int height)
{
    constexpr double view_width = 4.0;
    constexpr double center_x = -0.5;
    constexpr double center_y = 0.0;

    const double view_height = view_width * static_cast<double>(height) / static_cast<double>(width);

    return core::MandelParams{
        center_x - view_width / 2.0, center_y - view_height / 2.0, view_width / static_cast<double>(width),
        view_height / static_cast<double>(height),
        0.0,    // z0_real
        0.0,    // z0_imag
        4.0,    // bailout
        1.0e-7, // close_enough
        width, height,
        1000, // max_iterations
        15,   // first_saved_and
        10,   // periodicity_next_saved_incr
        1     // periodicity_check
    };
}

core::MandelPalette load_chroma_palette()
{
    const auto chroma_map = std::string{assets_dir} + "/chroma.map";
    std::ifstream input(chroma_map);
    if (!input)
    {
        throw std::runtime_error("Unable to open chroma.map");
    }

    auto palette = core::read_mandel_palette(input);
    if (palette.colors.empty())
    {
        throw std::runtime_error("chroma.map must contain RGB entries");
    }

    return palette;
}

std::size_t pixel_index(int width, int px, int py)
{
    return static_cast<std::size_t>(py) * static_cast<std::size_t>(width) + static_cast<std::size_t>(px);
}

wxBitmap make_bitmap(const core::MandelImage &image)
{
    wxImage wx_image(image.width, image.height);
    auto *data = wx_image.GetData();

    for (int py = 0; py < image.height; ++py)
    {
        for (int px = 0; px < image.width; ++px)
        {
            const auto color = image.pixels[pixel_index(image.width, px, py)];
            const auto offset = 3 * pixel_index(image.width, px, py);
            data[offset + 0] = color.red;
            data[offset + 1] = color.green;
            data[offset + 2] = color.blue;
        }
    }

    return wxBitmap(wx_image);
}

} // namespace

MandelRenderHost::MandelRenderHost(wxWindow &frame, wxWindow &cpu_display, wxsdl::SdlCanvas &canvas) :
    m_frame(frame),
    m_cpuDisplay(cpu_display),
    m_canvas(canvas),
    m_palette(load_chroma_palette()),
    m_timer(&frame)
{
    m_cpuDisplay.SetBackgroundStyle(wxBG_STYLE_PAINT);
    m_frame.Bind(wxEVT_SIZE, &MandelRenderHost::OnFrameSize, this);
    m_cpuDisplay.Bind(wxEVT_PAINT, &MandelRenderHost::OnCpuPaint, this);
    m_canvas.Bind(wxEVT_PAINT, &MandelRenderHost::OnCanvasPaint, this);
    m_frame.Bind(wxEVT_TIMER, &MandelRenderHost::OnTimer, this, m_timer.GetId());
    LayoutDisplays();
    SelectCpuPresentation();
    m_timer.Start(16);
}

MandelRenderHost::~MandelRenderHost()
{
    m_timer.Stop();
    m_frame.Unbind(wxEVT_TIMER, &MandelRenderHost::OnTimer, this, m_timer.GetId());
    m_canvas.Unbind(wxEVT_PAINT, &MandelRenderHost::OnCanvasPaint, this);
    m_cpuDisplay.Unbind(wxEVT_PAINT, &MandelRenderHost::OnCpuPaint, this);
    m_frame.Unbind(wxEVT_SIZE, &MandelRenderHost::OnFrameSize, this);
}

void MandelRenderHost::LayoutDisplays()
{
    const wxSize size = m_frame.GetClientSize();
    m_cpuDisplay.SetSize(0, 0, size.GetWidth(), size.GetHeight());
    m_canvas.SetSize(0, 0, size.GetWidth(), size.GetHeight());
    m_cpuDirty = true;
    m_cpuDisplay.Refresh(false);
}

void MandelRenderHost::RenderCpuImage()
{
    if (!m_cpuDirty)
    {
        return;
    }

    const wxSize size = m_cpuDisplay.GetClientSize();
    if (size.GetWidth() <= 0 || size.GetHeight() <= 0)
    {
        m_cpuBitmap = wxBitmap();
        m_cpuDirty = false;
        return;
    }

    const auto params = make_params(size.GetWidth(), size.GetHeight());
    const auto iterations = core::render_mandel_cpu(params);
    const auto image = core::map_mandel_colors(iterations, params.max_iterations, m_palette);
    m_cpuBitmap = make_bitmap(image);
    m_cpuDirty = false;
}

void MandelRenderHost::SelectCpuPresentation()
{
    m_canvas.Lower();
    m_cpuDisplay.Raise();
}

void MandelRenderHost::OnCanvasPaint(wxPaintEvent &)
{
    wxPaintDC paint{&m_canvas};
}

void MandelRenderHost::OnCpuPaint(wxPaintEvent &)
{
    wxPaintDC paint{&m_cpuDisplay};
    RenderCpuImage();
    if (m_cpuBitmap.IsOk())
    {
        paint.DrawBitmap(m_cpuBitmap, 0, 0, false);
    }
}

void MandelRenderHost::OnFrameSize(wxSizeEvent &event)
{
    LayoutDisplays();
    event.Skip();
}

void MandelRenderHost::OnTimer(wxTimerEvent &)
{
    m_cpuDisplay.Refresh(false);
}

} // namespace mandel
