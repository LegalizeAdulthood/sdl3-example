#include <sdlcpp/sdl.h>

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_hints.h>

#include <stdexcept>

namespace sdlcpp
{

std::string error_message(const char *operation)
{
    const char *error = SDL_GetError();
    std::string message = operation != nullptr ? operation : "SDL operation";
    message += " failed";
    if (error != nullptr && error[0] != '\0')
    {
        message += ": ";
        message += error;
    }
    return message;
}

void throw_error(const char *operation)
{
    throw Error(error_message(operation));
}

void check(bool ok, const char *operation)
{
    if (!ok)
    {
        throw_error(operation);
    }
}

void require_pointer(const void *pointer, const char *name)
{
    if (pointer == nullptr)
    {
        std::string message = name != nullptr ? name : "pointer";
        message += " must not be null";
        throw std::invalid_argument(message);
    }
}

bool SetHint(const char *name, const char *value)
{
    require_pointer(name, "name");
    require_pointer(value, "value");
    return SDL_SetHint(name, value);
}

void SetEventEnabled(Uint32 type, bool enabled)
{
    SDL_SetEventEnabled(type, enabled);
}

} // namespace sdlcpp
