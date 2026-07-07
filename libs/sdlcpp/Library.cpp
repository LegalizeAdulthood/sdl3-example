#include <sdlcpp/Library.h>

#include <sdlcpp/sdl.h>

#include <utility>

namespace core::sdl
{

Library::Library(SDL_InitFlags flags)
{
    check(SDL_Init(flags), "SDL_Init");
    initialized_ = true;
}

Library::~Library()
{
    reset();
}

Library::Library(Library &&other) noexcept :
    initialized_(std::exchange(other.initialized_, false))
{
}

Library &Library::operator=(Library &&other) noexcept
{
    if (this != &other)
    {
        reset();
        initialized_ = std::exchange(other.initialized_, false);
    }
    return *this;
}

void Library::reset() noexcept
{
    if (initialized_)
    {
        SDL_Quit();
        initialized_ = false;
    }
}

} // namespace core::sdl
