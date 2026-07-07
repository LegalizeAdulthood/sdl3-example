#pragma once

#include "sdlcpp/Error.h"

#include <string>

namespace core::sdl
{

[[nodiscard]] std::string error_message(const char *operation);
[[noreturn]] void throw_error(const char *operation);
void check(bool ok, const char *operation);
void require_pointer(const void *pointer, const char *name);

} // namespace core::sdl
