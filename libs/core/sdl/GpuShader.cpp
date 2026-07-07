#include <core/sdl/GpuShader.h>

#include <core/sdl/sdl.h>

namespace core::sdl
{

GpuShader make_gpu_shader(SDL_GPUDevice *device, const SDL_GPUShaderCreateInfo &create_info)
{
    require_pointer(device, "device");
    SDL_GPUShader *shader = SDL_CreateGPUShader(device, &create_info);
    if (shader == nullptr)
    {
        throw_error("SDL_CreateGPUShader");
    }
    return GpuShader(device, shader);
}

} // namespace core::sdl
