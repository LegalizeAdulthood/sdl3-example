#pragma once

#include <core/MandelImage.h>
#include <core/MandelViewport.h>

#include <sdlcpp/GpuDevice.h>
#include <sdlcpp/GpuWindowClaim.h>
#include <wxsdl/SdlCanvas.h>

#include <wx/bitmap.h>
#include <wx/gdicmn.h>
#include <wx/timer.h>
#include <wx/window.h>

namespace mandel
{

class MandelRenderHost
{
public:
    MandelRenderHost(wxWindow &frame, wxWindow &cpu_display, wxsdl::SdlCanvas &canvas);
    ~MandelRenderHost();

    void SelectCpuPresentation();
    void SelectGpuPresentation();

private:
    enum class Presentation
    {
        Cpu,
        Gpu
    };

    void ApplyPresentationVisibility();
    void BindMouseEvents(wxWindow &window);
    [[nodiscard]] wxSize DisplaySize() const;
    void InvalidateRender();
    void LayoutDisplays();
    void RefreshActiveDisplay();
    void RenderGpuFrame();
    void RenderCpuImage();
    void ReleaseMouse();
    void SubmitEmptyGpuCommandBuffer();
    void UnbindMouseEvents(wxWindow &window);

    void OnCanvasPaint(wxPaintEvent &event);
    void OnCpuPaint(wxPaintEvent &event);
    void OnFrameSize(wxSizeEvent &event);
    void OnMouseCaptureLost(wxMouseCaptureLostEvent &event);
    void OnMouseLeftDown(wxMouseEvent &event);
    void OnMouseLeftUp(wxMouseEvent &event);
    void OnMouseMove(wxMouseEvent &event);
    void OnMouseWheel(wxMouseEvent &event);
    void OnTimer(wxTimerEvent &event);

    wxWindow &m_frame;
    wxWindow &m_cpuDisplay;
    wxsdl::SdlCanvas &m_canvas;
    core::MandelPalette m_palette;
    core::MandelViewport m_viewport;
    Presentation m_presentation = Presentation::Cpu;
    sdlcpp::GpuDevice m_gpuDevice;
    sdlcpp::GpuWindowClaim m_gpuWindow;
    wxWindow *m_mouseCapture = nullptr;
    wxPoint m_lastMousePosition;
    wxTimer m_timer;
    wxBitmap m_cpuBitmap;
    bool m_cpuDirty = true;
};

} // namespace mandel
