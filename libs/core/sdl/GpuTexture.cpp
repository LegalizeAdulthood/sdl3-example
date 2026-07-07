#include <core/sdl/GpuTexture.h>

#include <core/sdl/sdl.h>

namespace core::sdl
{

GpuTexture make_gpu_texture(SDL_GPUDevice *device, const SDL_GPUTextureCreateInfo &create_info)
{
    require_pointer(device, "device");
    SDL_GPUTexture *texture = SDL_CreateGPUTexture(device, &create_info);
    if (texture == nullptr)
    {
        throw_error("SDL_CreateGPUTexture");
    }
    return GpuTexture(device, texture);
}

} // namespace core::sdl
