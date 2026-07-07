#include "SdlWindowAttach.h"

#include <sdlcpp/Window.h>

#include <wx/window.h>

namespace wxsdl::detail
{

sdlcpp::Window AttachSdlWindow(wxWindow &window)
{
    const wxSize size = window.GetClientSize();
    return sdlcpp::make_window_from_win32_hwnd(
        reinterpret_cast<void *>(window.GetHandle()),
        size.GetWidth(),
        size.GetHeight());
}

} // namespace wxsdl::detail
