#include "MandelRenderHost.h"

#include "assets.h"

#include <core/MandelCpu.h>
#include <core/MandelImage.h>

#include <sdlcpp/GpuCommandBuffer.h>
#include <sdlcpp/GpuRenderPass.h>
#include <sdlcpp/GpuSwapchainTexture.h>

#include <wx/dcclient.h>
#include <wx/event.h>
#include <wx/image.h>

#include <cmath>
#include <cstddef>
#include <fstream>
#include <stdexcept>
#include <string>

namespace mandel
{

namespace
{

constexpr double zoom_scale_per_wheel_click = 0.8;

constexpr SDL_GPUShaderFormat gpu_shader_formats =
    SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXIL | SDL_GPU_SHADERFORMAT_MSL;

constexpr SDL_FColor gpu_clear_color{0.0f, 1.0f, 0.0f, 1.0f};

wxWindow *event_window(wxEvent &event)
{
    return dynamic_cast<wxWindow *>(event.GetEventObject());
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
    m_gpuDevice(sdlcpp::make_gpu_device(gpu_shader_formats)),
    m_gpuWindow(sdlcpp::claim_window_for_gpu_device(m_gpuDevice.get(), m_canvas.window().get())),
    m_timer(&frame)
{
    m_cpuDisplay.SetBackgroundStyle(wxBG_STYLE_PAINT);
    m_frame.Bind(wxEVT_SIZE, &MandelRenderHost::OnFrameSize, this);
    m_cpuDisplay.Bind(wxEVT_PAINT, &MandelRenderHost::OnCpuPaint, this);
    m_canvas.Bind(wxEVT_PAINT, &MandelRenderHost::OnCanvasPaint, this);
    BindMouseEvents(m_cpuDisplay);
    BindMouseEvents(m_canvas);
    m_frame.Bind(wxEVT_TIMER, &MandelRenderHost::OnTimer, this, m_timer.GetId());
    LayoutDisplays();
    SelectCpuPresentation();
    SubmitEmptyGpuCommandBuffer();
    m_timer.Start(16);
}

MandelRenderHost::~MandelRenderHost()
{
    ReleaseMouse();
    m_timer.Stop();
    m_frame.Unbind(wxEVT_TIMER, &MandelRenderHost::OnTimer, this, m_timer.GetId());
    UnbindMouseEvents(m_canvas);
    UnbindMouseEvents(m_cpuDisplay);
    m_canvas.Unbind(wxEVT_PAINT, &MandelRenderHost::OnCanvasPaint, this);
    m_cpuDisplay.Unbind(wxEVT_PAINT, &MandelRenderHost::OnCpuPaint, this);
    m_frame.Unbind(wxEVT_SIZE, &MandelRenderHost::OnFrameSize, this);
}

void MandelRenderHost::BindMouseEvents(wxWindow &window)
{
    window.Bind(wxEVT_LEFT_DOWN, &MandelRenderHost::OnMouseLeftDown, this);
    window.Bind(wxEVT_LEFT_UP, &MandelRenderHost::OnMouseLeftUp, this);
    window.Bind(wxEVT_MOTION, &MandelRenderHost::OnMouseMove, this);
    window.Bind(wxEVT_MOUSEWHEEL, &MandelRenderHost::OnMouseWheel, this);
    window.Bind(wxEVT_MOUSE_CAPTURE_LOST, &MandelRenderHost::OnMouseCaptureLost, this);
}

wxSize MandelRenderHost::DisplaySize() const
{
    return m_frame.GetClientSize();
}

void MandelRenderHost::InvalidateRender()
{
    m_cpuDirty = true;
    RefreshActiveDisplay();
}

void MandelRenderHost::RefreshActiveDisplay()
{
    if (m_presentation == Presentation::Cpu)
    {
        m_cpuDisplay.Refresh(false);
    }
    else
    {
        m_canvas.Refresh(false);
    }
}

void MandelRenderHost::LayoutDisplays()
{
    const wxSize size = DisplaySize();
    m_cpuDisplay.SetSize(0, 0, size.GetWidth(), size.GetHeight());
    m_canvas.SetSize(0, 0, size.GetWidth(), size.GetHeight());
    ApplyPresentationVisibility();
    InvalidateRender();
}

void MandelRenderHost::ApplyPresentationVisibility()
{
    if (m_presentation == Presentation::Cpu)
    {
        m_canvas.Hide();
        m_cpuDisplay.Show();
    }
    else
    {
        m_cpuDisplay.Hide();
        m_canvas.Show();
    }
}

void MandelRenderHost::ReleaseMouse()
{
    if (m_mouseCapture != nullptr && m_mouseCapture->HasCapture())
    {
        m_mouseCapture->ReleaseMouse();
    }
    m_mouseCapture = nullptr;
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

    const auto params = m_viewport.params(size.GetWidth(), size.GetHeight());
    const auto iterations = core::render_mandel_cpu(params);
    const auto image = core::map_mandel_colors(iterations, params.max_iterations, m_palette);
    m_cpuBitmap = make_bitmap(image);
    m_cpuDirty = false;
}

void MandelRenderHost::RenderGpuFrame()
{
    auto command_buffer = sdlcpp::acquire_gpu_command_buffer(m_gpuDevice.get());
    const auto swapchain_texture =
        sdlcpp::wait_and_acquire_gpu_swapchain_texture(command_buffer.get(), m_canvas.window().get());

    if (swapchain_texture)
    {
        const SDL_GPUColorTargetInfo target_info{swapchain_texture.texture, 0, 0, gpu_clear_color, SDL_GPU_LOADOP_CLEAR,
            SDL_GPU_STOREOP_STORE, nullptr, 0, 0, false, false, 0, 0};
        sdlcpp::GpuRenderPass render_pass(command_buffer.get(), &target_info, 1);
    }

    command_buffer.submit();
}

void MandelRenderHost::SubmitEmptyGpuCommandBuffer()
{
    auto command_buffer = sdlcpp::acquire_gpu_command_buffer(m_gpuDevice.get());
    command_buffer.submit();
}

void MandelRenderHost::UnbindMouseEvents(wxWindow &window)
{
    window.Unbind(wxEVT_MOUSE_CAPTURE_LOST, &MandelRenderHost::OnMouseCaptureLost, this);
    window.Unbind(wxEVT_MOUSEWHEEL, &MandelRenderHost::OnMouseWheel, this);
    window.Unbind(wxEVT_MOTION, &MandelRenderHost::OnMouseMove, this);
    window.Unbind(wxEVT_LEFT_UP, &MandelRenderHost::OnMouseLeftUp, this);
    window.Unbind(wxEVT_LEFT_DOWN, &MandelRenderHost::OnMouseLeftDown, this);
}

void MandelRenderHost::SelectCpuPresentation()
{
    m_presentation = Presentation::Cpu;
    ApplyPresentationVisibility();
    RefreshActiveDisplay();
}

void MandelRenderHost::SelectGpuPresentation()
{
    m_presentation = Presentation::Gpu;
    ApplyPresentationVisibility();
    RefreshActiveDisplay();
}

void MandelRenderHost::OnCanvasPaint(wxPaintEvent &)
{
    wxPaintDC paint{&m_canvas};
    RenderGpuFrame();
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

void MandelRenderHost::OnMouseCaptureLost(wxMouseCaptureLostEvent &)
{
    m_mouseCapture = nullptr;
}

void MandelRenderHost::OnMouseLeftDown(wxMouseEvent &event)
{
    auto *window = event_window(event);
    if (window == nullptr)
    {
        return;
    }

    m_mouseCapture = window;
    m_lastMousePosition = event.GetPosition();
    if (!window->HasCapture())
    {
        window->CaptureMouse();
    }
}

void MandelRenderHost::OnMouseLeftUp(wxMouseEvent &)
{
    ReleaseMouse();
}

void MandelRenderHost::OnMouseMove(wxMouseEvent &event)
{
    if (m_mouseCapture == nullptr || !event.LeftIsDown())
    {
        return;
    }

    const wxPoint position = event.GetPosition();
    const int delta_x = position.x - m_lastMousePosition.x;
    const int delta_y = position.y - m_lastMousePosition.y;
    if (delta_x == 0 && delta_y == 0)
    {
        return;
    }

    const wxSize size = DisplaySize();
    m_viewport.pan_pixels(delta_x, delta_y, size.GetWidth(), size.GetHeight());
    m_lastMousePosition = position;
    InvalidateRender();
}

void MandelRenderHost::OnMouseWheel(wxMouseEvent &event)
{
    if (event.GetWheelDelta() == 0 || event.GetWheelRotation() == 0)
    {
        return;
    }

    const double clicks = static_cast<double>(event.GetWheelRotation()) / static_cast<double>(event.GetWheelDelta());
    const double scale = std::pow(zoom_scale_per_wheel_click, clicks);
    const wxPoint position = event.GetPosition();
    const wxSize size = DisplaySize();

    m_viewport.zoom_at(scale, position.x, position.y, size.GetWidth(), size.GetHeight());
    InvalidateRender();
}

void MandelRenderHost::OnTimer(wxTimerEvent &)
{
    RefreshActiveDisplay();
}

} // namespace mandel
