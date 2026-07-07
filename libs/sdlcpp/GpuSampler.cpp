#include <sdlcpp/GpuSampler.h>

#include <sdlcpp/sdl.h>

namespace sdlcpp
{

GpuSampler make_gpu_sampler(SDL_GPUDevice *device, const SDL_GPUSamplerCreateInfo &create_info)
{
    require_pointer(device, "device");
    SDL_GPUSampler *sampler = SDL_CreateGPUSampler(device, &create_info);
    if (sampler == nullptr)
    {
        throw_error("SDL_CreateGPUSampler");
    }
    return GpuSampler(device, sampler);
}

} // namespace sdlcpp
