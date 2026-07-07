#pragma once

#include <SDL3/SDL_stdinc.h>

#include <utility>

namespace sdlcpp
{

template <typename T, void(SDLCALL *Destroy)(T *)>
class Handle
{
public:
    using element_type = T;

    Handle() noexcept = default;

    explicit Handle(T *handle) noexcept :
        handle_(handle)
    {
    }

    ~Handle()
    {
        reset();
    }

    Handle(const Handle &) = delete;
    Handle &operator=(const Handle &) = delete;

    Handle(Handle &&other) noexcept :
        handle_(std::exchange(other.handle_, nullptr))
    {
    }

    Handle &operator=(Handle &&other) noexcept
    {
        if (this != &other)
        {
            reset();
            handle_ = std::exchange(other.handle_, nullptr);
        }
        return *this;
    }

    void reset(T *handle = nullptr) noexcept
    {
        if (handle_ != nullptr)
        {
            Destroy(handle_);
        }
        handle_ = handle;
    }

    [[nodiscard]] T *release() noexcept
    {
        return std::exchange(handle_, nullptr);
    }

    [[nodiscard]] T *get() const noexcept
    {
        return handle_;
    }

    explicit operator bool() const noexcept
    {
        return handle_ != nullptr;
    }

private:
    T *handle_ = nullptr;
};

} // namespace sdlcpp
