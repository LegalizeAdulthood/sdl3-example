#include <core/sdl/GpuBuffer.h>

#include <core/sdl/sdl.h>

namespace core::sdl
{

GpuBuffer make_gpu_buffer(SDL_GPUDevice *device, const SDL_GPUBufferCreateInfo &create_info)
{
    require_pointer(device, "device");
    SDL_GPUBuffer *buffer = SDL_CreateGPUBuffer(device, &create_info);
    if (buffer == nullptr)
    {
        throw_error("SDL_CreateGPUBuffer");
    }
    return GpuBuffer(device, buffer);
}

} // namespace core::sdl
