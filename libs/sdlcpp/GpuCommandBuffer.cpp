#include <sdlcpp/GpuCommandBuffer.h>

#include <sdlcpp/GpuComputePass.h>
#include <sdlcpp/GpuCopyPass.h>
#include <sdlcpp/GpuRenderPass.h>
#include <sdlcpp/GpuSwapchainTexture.h>
#include <sdlcpp/sdl.h>

#include <utility>

namespace sdlcpp
{

GpuCommandBuffer::GpuCommandBuffer(SDL_GPUDevice *device, SDL_GPUCommandBuffer *command_buffer) noexcept :
    device_(device),
    command_buffer_(command_buffer)
{
}

GpuCommandBuffer::~GpuCommandBuffer()
{
    reset();
}

GpuCommandBuffer::GpuCommandBuffer(GpuCommandBuffer &&other) noexcept :
    device_(std::exchange(other.device_, nullptr)),
    command_buffer_(std::exchange(other.command_buffer_, nullptr))
{
}

GpuCommandBuffer &GpuCommandBuffer::operator=(GpuCommandBuffer &&other) noexcept
{
    if (this != &other)
    {
        reset();
        device_ = std::exchange(other.device_, nullptr);
        command_buffer_ = std::exchange(other.command_buffer_, nullptr);
    }
    return *this;
}

void GpuCommandBuffer::reset() noexcept
{
    if (command_buffer_ != nullptr)
    {
        SDL_CancelGPUCommandBuffer(command_buffer_);
    }
    command_buffer_ = nullptr;
    device_ = nullptr;
}

void GpuCommandBuffer::CancelGPUCommandBuffer()
{
    SDL_GPUCommandBuffer *command_buffer = take();
    if (command_buffer != nullptr)
    {
        check(SDL_CancelGPUCommandBuffer(command_buffer), "SDL_CancelGPUCommandBuffer");
    }
}

void GpuCommandBuffer::SubmitGPUCommandBuffer()
{
    SDL_GPUCommandBuffer *command_buffer = take();
    require_pointer(command_buffer, "command_buffer");
    check(SDL_SubmitGPUCommandBuffer(command_buffer), "SDL_SubmitGPUCommandBuffer");
}

GpuFence GpuCommandBuffer::SubmitGPUCommandBufferAndAcquireFence()
{
    SDL_GPUDevice *device = device_;
    SDL_GPUCommandBuffer *command_buffer = take();
    require_pointer(command_buffer, "command_buffer");
    SDL_GPUFence *fence = SDL_SubmitGPUCommandBufferAndAcquireFence(command_buffer);
    if (fence == nullptr)
    {
        throw_error("SDL_SubmitGPUCommandBufferAndAcquireFence");
    }
    return GpuFence(device, fence);
}

SDL_GPUCommandBuffer *GpuCommandBuffer::release() noexcept
{
    return take();
}

GpuComputePass GpuCommandBuffer::BeginGPUComputePass(
    const SDL_GPUStorageTextureReadWriteBinding *storage_texture_bindings, Uint32 num_storage_texture_bindings,
    const SDL_GPUStorageBufferReadWriteBinding *storage_buffer_bindings, Uint32 num_storage_buffer_bindings)
{
    require_pointer(command_buffer_, "command_buffer");
    SDL_GPUComputePass *compute_pass = SDL_BeginGPUComputePass(command_buffer_, storage_texture_bindings,
        num_storage_texture_bindings, storage_buffer_bindings, num_storage_buffer_bindings);
    if (compute_pass == nullptr)
    {
        throw_error("SDL_BeginGPUComputePass");
    }
    return GpuComputePass(compute_pass);
}

GpuCopyPass GpuCommandBuffer::BeginGPUCopyPass()
{
    require_pointer(command_buffer_, "command_buffer");
    SDL_GPUCopyPass *copy_pass = SDL_BeginGPUCopyPass(command_buffer_);
    if (copy_pass == nullptr)
    {
        throw_error("SDL_BeginGPUCopyPass");
    }
    return GpuCopyPass(copy_pass);
}

GpuRenderPass GpuCommandBuffer::BeginGPURenderPass(const SDL_GPUColorTargetInfo *color_target_infos,
    Uint32 num_color_targets, const SDL_GPUDepthStencilTargetInfo *depth_stencil_target_info)
{
    require_pointer(command_buffer_, "command_buffer");
    require_pointer(color_target_infos, "color_target_infos");
    SDL_GPURenderPass *render_pass =
        SDL_BeginGPURenderPass(command_buffer_, color_target_infos, num_color_targets, depth_stencil_target_info);
    if (render_pass == nullptr)
    {
        throw_error("SDL_BeginGPURenderPass");
    }
    return GpuRenderPass(render_pass);
}

void GpuCommandBuffer::PushGPUVertexUniformData(Uint32 slot_index, const void *data, Uint32 length)
{
    require_pointer(command_buffer_, "command_buffer");
    require_pointer(data, "data");
    SDL_PushGPUVertexUniformData(command_buffer_, slot_index, data, length);
}

void GpuCommandBuffer::PushGPUFragmentUniformData(Uint32 slot_index, const void *data, Uint32 length)
{
    require_pointer(command_buffer_, "command_buffer");
    require_pointer(data, "data");
    SDL_PushGPUFragmentUniformData(command_buffer_, slot_index, data, length);
}

void GpuCommandBuffer::PushGPUComputeUniformData(Uint32 slot_index, const void *data, Uint32 length)
{
    require_pointer(command_buffer_, "command_buffer");
    require_pointer(data, "data");
    SDL_PushGPUComputeUniformData(command_buffer_, slot_index, data, length);
}

GpuSwapchainTexture GpuCommandBuffer::AcquireGPUSwapchainTexture(SDL_Window *window)
{
    require_pointer(command_buffer_, "command_buffer");
    return acquire_gpu_swapchain_texture(command_buffer_, window);
}

GpuSwapchainTexture GpuCommandBuffer::WaitAndAcquireGPUSwapchainTexture(SDL_Window *window)
{
    require_pointer(command_buffer_, "command_buffer");
    return wait_and_acquire_gpu_swapchain_texture(command_buffer_, window);
}

SDL_GPUCommandBuffer *GpuCommandBuffer::take() noexcept
{
    device_ = nullptr;
    return std::exchange(command_buffer_, nullptr);
}

GpuCommandBuffer acquire_gpu_command_buffer(SDL_GPUDevice *device)
{
    require_pointer(device, "device");
    SDL_GPUCommandBuffer *command_buffer = SDL_AcquireGPUCommandBuffer(device);
    if (command_buffer == nullptr)
    {
        throw_error("SDL_AcquireGPUCommandBuffer");
    }
    return GpuCommandBuffer(device, command_buffer);
}

} // namespace sdlcpp
