#include <sdlcpp/GpuCommandBuffer.h>

#include <sdlcpp/sdl.h>

#include <utility>

namespace core::sdl
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

void GpuCommandBuffer::cancel()
{
    SDL_GPUCommandBuffer *command_buffer = take();
    if (command_buffer != nullptr)
    {
        check(SDL_CancelGPUCommandBuffer(command_buffer), "SDL_CancelGPUCommandBuffer");
    }
}

void GpuCommandBuffer::submit()
{
    SDL_GPUCommandBuffer *command_buffer = take();
    require_pointer(command_buffer, "command_buffer");
    check(SDL_SubmitGPUCommandBuffer(command_buffer), "SDL_SubmitGPUCommandBuffer");
}

GpuFence GpuCommandBuffer::submit_and_acquire_fence()
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

} // namespace core::sdl
