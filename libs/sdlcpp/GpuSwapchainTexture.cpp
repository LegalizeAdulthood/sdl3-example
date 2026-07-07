#include <sdlcpp/GpuSwapchainTexture.h>

#include <sdlcpp/sdl.h>

namespace sdlcpp
{

GpuSwapchainTexture acquire_gpu_swapchain_texture(SDL_GPUCommandBuffer *command_buffer, SDL_Window *window)
{
    require_pointer(command_buffer, "command_buffer");
    require_pointer(window, "window");

    GpuSwapchainTexture swapchain_texture;
    check(SDL_AcquireGPUSwapchainTexture(
              command_buffer, window, &swapchain_texture.texture, &swapchain_texture.width, &swapchain_texture.height),
        "SDL_AcquireGPUSwapchainTexture");
    return swapchain_texture;
}

GpuSwapchainTexture wait_and_acquire_gpu_swapchain_texture(SDL_GPUCommandBuffer *command_buffer, SDL_Window *window)
{
    require_pointer(command_buffer, "command_buffer");
    require_pointer(window, "window");

    GpuSwapchainTexture swapchain_texture;
    check(SDL_WaitAndAcquireGPUSwapchainTexture(
              command_buffer, window, &swapchain_texture.texture, &swapchain_texture.width, &swapchain_texture.height),
        "SDL_WaitAndAcquireGPUSwapchainTexture");
    return swapchain_texture;
}

} // namespace sdlcpp
