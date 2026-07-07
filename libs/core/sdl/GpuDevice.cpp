#include <core/sdl/GpuDevice.h>

#include <core/sdl/sdl.h>

namespace core::sdl
{

GpuDevice make_gpu_device(SDL_GPUShaderFormat format_flags, bool debug_mode, const char *name)
{
    SDL_GPUDevice *device = SDL_CreateGPUDevice(format_flags, debug_mode, name);
    if (device == nullptr)
    {
        throw_error("SDL_CreateGPUDevice");
    }
    return GpuDevice(device);
}

} // namespace core::sdl
