#pragma once

#include <wxsdl/SdlCanvas.h>

#include <wx/timer.h>

namespace mandel
{

class MandelRenderHost
{
public:
    explicit MandelRenderHost(wxsdl::SdlCanvas &canvas);
    ~MandelRenderHost();

private:
    void OnPaint(wxPaintEvent &event);
    void OnSize(wxSizeEvent &event);
    void OnTimer(wxTimerEvent &event);

    wxsdl::SdlCanvas &m_canvas;
    wxTimer m_timer;
};

} // namespace mandel
