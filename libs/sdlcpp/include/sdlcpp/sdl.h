#pragma once

#include "sdlcpp/Error.h"

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_hints.h>

#include <string>

namespace sdlcpp
{

[[nodiscard]] std::string error_message(const char *operation);
[[noreturn]] void throw_error(const char *operation);
void check(bool ok, const char *operation);
void require_pointer(const void *pointer, const char *name);
bool SetHint(const char *name, const char *value);
void SetEventEnabled(Uint32 type, bool enabled);

} // namespace sdlcpp
