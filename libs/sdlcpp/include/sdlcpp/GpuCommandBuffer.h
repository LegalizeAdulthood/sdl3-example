#pragma once

#include "sdlcpp/GpuFence.h"

#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_video.h>

namespace sdlcpp
{

class GpuComputePass;
class GpuCopyPass;
class GpuRenderPass;
struct GpuSwapchainTexture;

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
    void CancelGPUCommandBuffer();
    void SubmitGPUCommandBuffer();
    [[nodiscard]] GpuFence SubmitGPUCommandBufferAndAcquireFence();
    [[nodiscard]] SDL_GPUCommandBuffer *release() noexcept;
    [[nodiscard]] GpuComputePass BeginGPUComputePass(
        const SDL_GPUStorageTextureReadWriteBinding *storage_texture_bindings = nullptr,
        Uint32 num_storage_texture_bindings = 0,
        const SDL_GPUStorageBufferReadWriteBinding *storage_buffer_bindings = nullptr,
        Uint32 num_storage_buffer_bindings = 0);
    [[nodiscard]] GpuCopyPass BeginGPUCopyPass();
    [[nodiscard]] GpuRenderPass BeginGPURenderPass(const SDL_GPUColorTargetInfo *color_target_infos,
        Uint32 num_color_targets, const SDL_GPUDepthStencilTargetInfo *depth_stencil_target_info = nullptr);
    void PushGPUVertexUniformData(Uint32 slot_index, const void *data, Uint32 length);
    void PushGPUFragmentUniformData(Uint32 slot_index, const void *data, Uint32 length);
    void PushGPUComputeUniformData(Uint32 slot_index, const void *data, Uint32 length);
    [[nodiscard]] GpuSwapchainTexture AcquireGPUSwapchainTexture(SDL_Window *window);
    [[nodiscard]] GpuSwapchainTexture WaitAndAcquireGPUSwapchainTexture(SDL_Window *window);

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

} // namespace sdlcpp
