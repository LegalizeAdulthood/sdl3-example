#include <sdlcpp/GpuTexture.h>

#include <sdlcpp/sdl.h>

namespace sdlcpp
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

} // namespace sdlcpp
