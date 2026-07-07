#pragma once

#include <SDL3/SDL_init.h>

namespace core::sdl
{

class Library
{
public:
    explicit Library(SDL_InitFlags flags);
    ~Library();

    Library(const Library &) = delete;
    Library &operator=(const Library &) = delete;

    Library(Library &&other) noexcept;
    Library &operator=(Library &&other) noexcept;

    void reset() noexcept;

    [[nodiscard]] bool initialized() const noexcept
    {
        return initialized_;
    }

private:
    bool initialized_ = false;
};

} // namespace core::sdl
