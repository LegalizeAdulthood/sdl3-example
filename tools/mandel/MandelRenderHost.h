#pragma once

#include <core/MandelImage.h>

#include <wxsdl/SdlCanvas.h>

#include <wx/bitmap.h>
#include <wx/timer.h>
#include <wx/window.h>

namespace mandel
{

class MandelRenderHost
{
public:
    MandelRenderHost(wxWindow &frame, wxWindow &cpu_display, wxsdl::SdlCanvas &canvas);
    ~MandelRenderHost();

private:
    void LayoutDisplays();
    void RenderCpuImage();
    void SelectCpuPresentation();

    void OnCanvasPaint(wxPaintEvent &event);
    void OnCpuPaint(wxPaintEvent &event);
    void OnFrameSize(wxSizeEvent &event);
    void OnTimer(wxTimerEvent &event);

    wxWindow &m_frame;
    wxWindow &m_cpuDisplay;
    wxsdl::SdlCanvas &m_canvas;
    core::MandelPalette m_palette;
    wxTimer m_timer;
    wxBitmap m_cpuBitmap;
    bool m_cpuDirty = true;
};

} // namespace mandel
