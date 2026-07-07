#include "MandelRenderHost.h"

#include <wx/dcclient.h>

namespace mandel
{

MandelRenderHost::MandelRenderHost(wxsdl::SdlCanvas &canvas) :
    m_canvas(canvas),
    m_timer(&canvas)
{
    m_canvas.Bind(wxEVT_PAINT, &MandelRenderHost::OnPaint, this);
    m_canvas.Bind(wxEVT_SIZE, &MandelRenderHost::OnSize, this);
    m_canvas.Bind(wxEVT_TIMER, &MandelRenderHost::OnTimer, this, m_timer.GetId());
    m_timer.Start(16);
}

MandelRenderHost::~MandelRenderHost()
{
    m_timer.Stop();
    m_canvas.Unbind(wxEVT_TIMER, &MandelRenderHost::OnTimer, this, m_timer.GetId());
    m_canvas.Unbind(wxEVT_SIZE, &MandelRenderHost::OnSize, this);
    m_canvas.Unbind(wxEVT_PAINT, &MandelRenderHost::OnPaint, this);
}

void MandelRenderHost::OnPaint(wxPaintEvent &)
{
    wxPaintDC paint{&m_canvas};
}

void MandelRenderHost::OnSize(wxSizeEvent &event)
{
    m_canvas.Refresh(false);
    event.Skip();
}

void MandelRenderHost::OnTimer(wxTimerEvent &)
{
    m_canvas.Refresh(false);
}

} // namespace mandel
