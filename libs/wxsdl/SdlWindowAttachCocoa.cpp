#include "SdlWindowAttach.h"

#include <sdlcpp/Window.h>

#include <wx/window.h>

namespace wxsdl::detail
{

sdlcpp::Window AttachSdlWindow(wxWindow &window)
{
    const wxSize size = window.GetClientSize();
    return sdlcpp::make_window_from_cocoa_view(
        reinterpret_cast<void *>(window.GetHandle()),
        size.GetWidth(),
        size.GetHeight());
}

} // namespace wxsdl::detail
