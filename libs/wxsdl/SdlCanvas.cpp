#include <wxsdl/SdlCanvas.h>

#include "SdlWindowAttach.h"

namespace wxsdl
{

SdlCanvas::SdlCanvas(
    wxWindow *parent,
    wxWindowID id,
    const wxPoint &pos,
    const wxSize &size,
    long style) :
    wxWindow(parent, id, pos, size, style)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    m_window = detail::AttachSdlWindow(*this);
}

SdlCanvas::~SdlCanvas()
{
    m_window.reset();
}

} // namespace wxsdl
