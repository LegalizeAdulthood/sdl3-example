#pragma once

#include "core/sdl/GpuFence.h"

#include <SDL3/SDL_gpu.h>

namespace core::sdl
{

class GpuCommandBuffer
{
public:
    GpuCommandBuffer() noexcept = default;
    GpuCommandBuffer(SDL_GPUDevice *device, SDL_GPUCommandBuffer *command_buffer) noexcept;
    ~GpuCommandBuffer();

    GpuCommandBuffer(const GpuCommandBuffer &) = delete;
    GpuCommandBuffer &operator=(const GpuCommandBuffer &) = delete;

    GpuCommandBuffer(GpuCommandBuffer &&other) noexcept;
    GpuCommandBuffer &operator=(GpuCommandBuffer &&other) noexcept;

    void reset() noexcept;
    void cancel();
    void submit();
    [[nodiscard]] GpuFence submit_and_acquire_fence();
    [[nodiscard]] SDL_GPUCommandBuffer *release() noexcept;

    [[nodiscard]] SDL_GPUDevice *device() const noexcept
    {
        return device_;
    }

    [[nodiscard]] SDL_GPUCommandBuffer *get() const noexcept
    {
        return command_buffer_;
    }

    explicit operator bool() const noexcept
    {
        return command_buffer_ != nullptr;
    }

private:
    [[nodiscard]] SDL_GPUCommandBuffer *take() noexcept;

    SDL_GPUDevice *device_ = nullptr;
    SDL_GPUCommandBuffer *command_buffer_ = nullptr;
};

[[nodiscard]] GpuCommandBuffer acquire_gpu_command_buffer(SDL_GPUDevice *device);

} // namespace core::sdl
