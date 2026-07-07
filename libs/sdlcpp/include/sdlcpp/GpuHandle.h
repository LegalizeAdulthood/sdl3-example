#pragma once

#include <SDL3/SDL_gpu.h>

#include <utility>

namespace sdlcpp
{

template <typename T, void(SDLCALL *Release)(SDL_GPUDevice *, T *)>
class GpuHandle
{
public:
    using element_type = T;

    GpuHandle() noexcept = default;

    GpuHandle(SDL_GPUDevice *device, T *handle) noexcept :
        device_(device),
        handle_(handle)
    {
    }

    ~GpuHandle()
    {
        reset();
    }

    GpuHandle(const GpuHandle &) = delete;
    GpuHandle &operator=(const GpuHandle &) = delete;

    GpuHandle(GpuHandle &&other) noexcept :
        device_(std::exchange(other.device_, nullptr)),
        handle_(std::exchange(other.handle_, nullptr))
    {
    }

    GpuHandle &operator=(GpuHandle &&other) noexcept
    {
        if (this != &other)
        {
            reset();
            device_ = std::exchange(other.device_, nullptr);
            handle_ = std::exchange(other.handle_, nullptr);
        }
        return *this;
    }

    void reset() noexcept
    {
        if (handle_ != nullptr)
        {
            Release(device_, handle_);
            handle_ = nullptr;
        }
        device_ = nullptr;
    }

    void reset(SDL_GPUDevice *device, T *handle) noexcept
    {
        reset();
        device_ = device;
        handle_ = handle;
    }

    [[nodiscard]] T *release() noexcept
    {
        device_ = nullptr;
        return std::exchange(handle_, nullptr);
    }

    [[nodiscard]] SDL_GPUDevice *device() const noexcept
    {
        return device_;
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
    SDL_GPUDevice *device_ = nullptr;
    T *handle_ = nullptr;
};

} // namespace sdlcpp
