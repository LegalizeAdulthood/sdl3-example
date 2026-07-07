#pragma once

#include <sdlcpp/Window.h>

class wxWindow;

namespace wxsdl::detail
{

[[nodiscard]] sdlcpp::Window AttachSdlWindow(wxWindow &window);

} // namespace wxsdl::detail
