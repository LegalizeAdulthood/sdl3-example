#pragma once

#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_video.h>

namespace core::sdl
{

struct GpuSwapchainTexture
{
    SDL_GPUTexture *texture = nullptr;
    Uint32 width = 0;
    Uint32 height = 0;

    explicit operator bool() const noexcept
    {
        return texture != nullptr;
    }
};

[[nodiscard]] GpuSwapchainTexture acquire_gpu_swapchain_texture(
    SDL_GPUCommandBuffer *command_buffer, SDL_Window *window);

[[nodiscard]] GpuSwapchainTexture wait_and_acquire_gpu_swapchain_texture(
    SDL_GPUCommandBuffer *command_buffer, SDL_Window *window);

} // namespace core::sdl
