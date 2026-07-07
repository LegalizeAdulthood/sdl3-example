#include "SdlWindowAttach.h"

#include <sdlcpp/Window.h>
#include <sdlcpp/sdl.h>

#include <gdk/gdkx.h>
#include <gtk/gtk.h>
#include <wx/window.h>

#include <stdexcept>

namespace wxsdl::detail
{

sdlcpp::Window AttachSdlWindow(wxWindow &window)
{
    auto *widget = static_cast<GtkWidget *>(window.GetHandle());
    sdlcpp::require_pointer(widget, "GTK widget");

    if (!gtk_widget_get_realized(widget))
    {
        gtk_widget_realize(widget);
    }

    GdkWindow *gdkWindow = gtk_widget_get_window(widget);
    sdlcpp::require_pointer(gdkWindow, "GDK window");

    if (!GDK_IS_X11_WINDOW(gdkWindow))
    {
        throw std::runtime_error("wx GTK window is not an X11 window");
    }

    const wxSize size = window.GetClientSize();
    return sdlcpp::make_window_from_x11_window(
        gdk_x11_window_get_xid(gdkWindow),
        size.GetWidth(),
        size.GetHeight());
}

} // namespace wxsdl::detail
