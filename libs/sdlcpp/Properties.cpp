#include <sdlcpp/Properties.h>

#include <sdlcpp/sdl.h>

#include <utility>

namespace sdlcpp
{

Properties::Properties(SDL_PropertiesID properties) noexcept :
    properties_(properties)
{
}

Properties Properties::create()
{
    SDL_PropertiesID properties = SDL_CreateProperties();
    if (properties == 0)
    {
        throw_error("SDL_CreateProperties");
    }
    return Properties(properties);
}

Properties::~Properties()
{
    reset();
}

Properties::Properties(Properties &&other) noexcept :
    properties_(std::exchange(other.properties_, 0))
{
}

Properties &Properties::operator=(Properties &&other) noexcept
{
    if (this != &other)
    {
        reset();
        properties_ = std::exchange(other.properties_, 0);
    }
    return *this;
}

void Properties::reset(SDL_PropertiesID properties) noexcept
{
    if (properties_ != 0)
    {
        SDL_DestroyProperties(properties_);
    }
    properties_ = properties;
}

SDL_PropertiesID Properties::release() noexcept
{
    return std::exchange(properties_, 0);
}

void Properties::set_boolean(const char *name, bool value)
{
    check(SDL_SetBooleanProperty(properties_, name, value), "SDL_SetBooleanProperty");
}

void Properties::set_number(const char *name, Sint64 value)
{
    check(SDL_SetNumberProperty(properties_, name, value), "SDL_SetNumberProperty");
}

void Properties::set_pointer(const char *name, void *value)
{
    check(SDL_SetPointerProperty(properties_, name, value), "SDL_SetPointerProperty");
}

void Properties::set_string(const char *name, const char *value)
{
    check(SDL_SetStringProperty(properties_, name, value), "SDL_SetStringProperty");
}

} // namespace sdlcpp
