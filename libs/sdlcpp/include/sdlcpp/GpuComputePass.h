#pragma once

#include <SDL3/SDL_gpu.h>

namespace sdlcpp
{

class GpuComputePass
{
public:
    GpuComputePass() noexcept = default;
    GpuComputePass(SDL_GPUCommandBuffer *command_buffer,
        const SDL_GPUStorageTextureReadWriteBinding *storage_texture_bindings = nullptr,
        Uint32 num_storage_texture_bindings = 0,
        const SDL_GPUStorageBufferReadWriteBinding *storage_buffer_bindings = nullptr,
        Uint32 num_storage_buffer_bindings = 0);
    ~GpuComputePass();

    GpuComputePass(const GpuComputePass &) = delete;
    GpuComputePass &operator=(const GpuComputePass &) = delete;

    GpuComputePass(GpuComputePass &&other) noexcept;
    GpuComputePass &operator=(GpuComputePass &&other) noexcept;

    void begin(SDL_GPUCommandBuffer *command_buffer,
        const SDL_GPUStorageTextureReadWriteBinding *storage_texture_bindings = nullptr,
        Uint32 num_storage_texture_bindings = 0,
        const SDL_GPUStorageBufferReadWriteBinding *storage_buffer_bindings = nullptr,
        Uint32 num_storage_buffer_bindings = 0);
    void end() noexcept;

    [[nodiscard]] SDL_GPUComputePass *get() const noexcept
    {
        return compute_pass_;
    }

    explicit operator bool() const noexcept
    {
        return compute_pass_ != nullptr;
    }

private:
    SDL_GPUComputePass *compute_pass_ = nullptr;
};

} // namespace sdlcpp
