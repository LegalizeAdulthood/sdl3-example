#pragma once

#include "sdlcpp/Handle.h"
#include "sdlcpp/Properties.h"

#include <SDL3/SDL_video.h>

#include <cstdint>

namespace sdlcpp
{

using Window = Handle<SDL_Window, SDL_DestroyWindow>;

[[nodiscard]] Window make_window_from_properties(SDL_PropertiesID properties);
[[nodiscard]] Window make_window_from_properties(const Properties &properties);

void set_wrapped_window_properties(Properties &properties, int width, int height, const char *title = nullptr);

[[nodiscard]] Window make_window_from_win32_hwnd(void *hwnd, int width, int height, const char *title = nullptr);

[[nodiscard]] Window make_window_from_cocoa_view(void *ns_view, int width, int height, const char *title = nullptr);

[[nodiscard]] Window make_window_from_wayland_surface(
    void *wl_surface, int width, int height, const char *title = nullptr);

[[nodiscard]] Window make_window_from_x11_window(
    std::uint64_t window, int width, int height, const char *title = nullptr);

} // namespace sdlcpp
