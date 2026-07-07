#pragma once

#include "sdlcpp/GpuBuffer.h"
#include "sdlcpp/GpuTransferBuffer.h"

#include <SDL3/SDL_gpu.h>

namespace sdlcpp
{

class GpuCommandBuffer;

class GpuCopyPass
{
public:
    GpuCopyPass() noexcept = default;
    explicit GpuCopyPass(SDL_GPUCopyPass *copy_pass) noexcept;
    ~GpuCopyPass();

    GpuCopyPass(const GpuCopyPass &) = delete;
    GpuCopyPass &operator=(const GpuCopyPass &) = delete;

    GpuCopyPass(GpuCopyPass &&other) noexcept;
    GpuCopyPass &operator=(GpuCopyPass &&other) noexcept;

    void EndGPUCopyPass() noexcept;
    void UploadToGPUBuffer(
        const SDL_GPUTransferBufferLocation *source, const SDL_GPUBufferRegion *destination, bool cycle);
    void UploadToGPUBuffer(const GpuTransferBuffer &source_buffer, Uint32 source_offset,
        const GpuBuffer &destination_buffer, Uint32 destination_offset, Uint32 size, bool cycle);

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

} // namespace sdlcpp
