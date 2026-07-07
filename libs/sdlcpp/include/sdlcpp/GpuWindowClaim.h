#pragma once

#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_video.h>

namespace sdlcpp
{

class GpuWindowClaim
{
public:
    GpuWindowClaim() noexcept = default;
    GpuWindowClaim(SDL_GPUDevice *device, SDL_Window *window);
    ~GpuWindowClaim();

    GpuWindowClaim(const GpuWindowClaim &) = delete;
    GpuWindowClaim &operator=(const GpuWindowClaim &) = delete;

    GpuWindowClaim(GpuWindowClaim &&other) noexcept;
    GpuWindowClaim &operator=(GpuWindowClaim &&other) noexcept;

    void claim(SDL_GPUDevice *device, SDL_Window *window);
    void reset() noexcept;

    [[nodiscard]] SDL_GPUDevice *device() const noexcept
    {
        return device_;
    }

    [[nodiscard]] SDL_Window *window() const noexcept
    {
        return window_;
    }

    explicit operator bool() const noexcept
    {
        return device_ != nullptr && window_ != nullptr;
    }

private:
    SDL_GPUDevice *device_ = nullptr;
    SDL_Window *window_ = nullptr;
};

[[nodiscard]] GpuWindowClaim claim_window_for_gpu_device(SDL_GPUDevice *device, SDL_Window *window);

} // namespace sdlcpp
