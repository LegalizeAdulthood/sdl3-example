#include <core/sdl/GpuWindowClaim.h>

#include <core/sdl/sdl.h>

#include <utility>

namespace core::sdl
{

GpuWindowClaim::GpuWindowClaim(SDL_GPUDevice *device, SDL_Window *window)
{
    claim(device, window);
}

GpuWindowClaim::~GpuWindowClaim()
{
    reset();
}

GpuWindowClaim::GpuWindowClaim(GpuWindowClaim &&other) noexcept :
    device_(std::exchange(other.device_, nullptr)),
    window_(std::exchange(other.window_, nullptr))
{
}

GpuWindowClaim &GpuWindowClaim::operator=(GpuWindowClaim &&other) noexcept
{
    if (this != &other)
    {
        reset();
        device_ = std::exchange(other.device_, nullptr);
        window_ = std::exchange(other.window_, nullptr);
    }
    return *this;
}

void GpuWindowClaim::claim(SDL_GPUDevice *device, SDL_Window *window)
{
    require_pointer(device, "device");
    require_pointer(window, "window");
    reset();
    check(SDL_ClaimWindowForGPUDevice(device, window), "SDL_ClaimWindowForGPUDevice");
    device_ = device;
    window_ = window;
}

void GpuWindowClaim::reset() noexcept
{
    if (device_ != nullptr && window_ != nullptr)
    {
        SDL_ReleaseWindowFromGPUDevice(device_, window_);
    }
    device_ = nullptr;
    window_ = nullptr;
}

GpuWindowClaim claim_window_for_gpu_device(SDL_GPUDevice *device, SDL_Window *window)
{
    return GpuWindowClaim(device, window);
}

} // namespace core::sdl
