#pragma once

#include <SDL3/SDL_gpu.h>

namespace core::sdl
{

class GpuCopyPass
{
public:
    GpuCopyPass() noexcept = default;
    explicit GpuCopyPass(SDL_GPUCommandBuffer *command_buffer);
    ~GpuCopyPass();

    GpuCopyPass(const GpuCopyPass &) = delete;
    GpuCopyPass &operator=(const GpuCopyPass &) = delete;

    GpuCopyPass(GpuCopyPass &&other) noexcept;
    GpuCopyPass &operator=(GpuCopyPass &&other) noexcept;

    void begin(SDL_GPUCommandBuffer *command_buffer);
    void end() noexcept;

    [[nodiscard]] SDL_GPUCopyPass *get() const noexcept
    {
        return copy_pass_;
    }

    explicit operator bool() const noexcept
    {
        return copy_pass_ != nullptr;
    }

private:
    SDL_GPUCopyPass *copy_pass_ = nullptr;
};

} // namespace core::sdl
