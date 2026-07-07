#include <sdlcpp/Window.h>

#include <sdlcpp/sdl.h>

#include <stdexcept>

namespace core::sdl
{

Window make_window_from_properties(SDL_PropertiesID properties)
{
    if (properties == 0)
    {
        throw std::invalid_argument("properties must not be 0");
    }

    SDL_Window *window = SDL_CreateWindowWithProperties(properties);
    if (window == nullptr)
    {
        throw_error("SDL_CreateWindowWithProperties");
    }
    return Window(window);
}

Window make_window_from_properties(const Properties &properties)
{
    return make_window_from_properties(properties.get());
}

void set_wrapped_window_properties(Properties &properties, int width, int height, const char *title)
{
    if (width > 0)
    {
        properties.set_number(SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, width);
    }
    if (height > 0)
    {
        properties.set_number(SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, height);
    }
    if (title != nullptr)
    {
        properties.set_string(SDL_PROP_WINDOW_CREATE_TITLE_STRING, title);
    }
}

Window make_window_from_win32_hwnd(void *hwnd, int width, int height, const char *title)
{
    require_pointer(hwnd, "hwnd");

    Properties properties = Properties::create();
    properties.set_pointer(SDL_PROP_WINDOW_CREATE_WIN32_HWND_POINTER, hwnd);
    set_wrapped_window_properties(properties, width, height, title);
    return make_window_from_properties(properties);
}

Window make_window_from_cocoa_view(void *ns_view, int width, int height, const char *title)
{
    require_pointer(ns_view, "ns_view");

    Properties properties = Properties::create();
    properties.set_pointer(SDL_PROP_WINDOW_CREATE_COCOA_VIEW_POINTER, ns_view);
    set_wrapped_window_properties(properties, width, height, title);
    return make_window_from_properties(properties);
}

Window make_window_from_wayland_surface(void *wl_surface, int width, int height, const char *title)
{
    require_pointer(wl_surface, "wl_surface");

    Properties properties = Properties::create();
    properties.set_pointer(SDL_PROP_WINDOW_CREATE_WAYLAND_WL_SURFACE_POINTER, wl_surface);
    set_wrapped_window_properties(properties, width, height, title);
    return make_window_from_properties(properties);
}

Window make_window_from_x11_window(std::uint64_t window, int width, int height, const char *title)
{
    if (window == 0)
    {
        throw std::invalid_argument("window must not be 0");
    }

    Properties properties = Properties::create();
    properties.set_number(SDL_PROP_WINDOW_CREATE_X11_WINDOW_NUMBER, static_cast<Sint64>(window));
    set_wrapped_window_properties(properties, width, height, title);
    return make_window_from_properties(properties);
}

} // namespace core::sdl
