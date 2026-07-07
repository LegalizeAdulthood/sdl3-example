#pragma once

#include <SDL3/SDL_properties.h>

namespace core::sdl
{

class Properties
{
public:
    Properties() noexcept = default;
    explicit Properties(SDL_PropertiesID properties) noexcept;
    static Properties create();

    ~Properties();

    Properties(const Properties &) = delete;
    Properties &operator=(const Properties &) = delete;

    Properties(Properties &&other) noexcept;
    Properties &operator=(Properties &&other) noexcept;

    void reset(SDL_PropertiesID properties = 0) noexcept;

    [[nodiscard]] SDL_PropertiesID release() noexcept;

    [[nodiscard]] SDL_PropertiesID get() const noexcept
    {
        return properties_;
    }

    void set_boolean(const char *name, bool value);
    void set_number(const char *name, Sint64 value);
    void set_pointer(const char *name, void *value);
    void set_string(const char *name, const char *value);

    explicit operator bool() const noexcept
    {
        return properties_ != 0;
    }

private:
    SDL_PropertiesID properties_ = 0;
};

} // namespace core::sdl
