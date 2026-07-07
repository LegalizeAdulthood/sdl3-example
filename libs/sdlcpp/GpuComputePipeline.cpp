#include <sdlcpp/GpuComputePipeline.h>

#include <sdlcpp/sdl.h>

namespace core::sdl
{

GpuComputePipeline make_gpu_compute_pipeline(SDL_GPUDevice *device, const SDL_GPUComputePipelineCreateInfo &create_info)
{
    require_pointer(device, "device");
    SDL_GPUComputePipeline *pipeline = SDL_CreateGPUComputePipeline(device, &create_info);
    if (pipeline == nullptr)
    {
        throw_error("SDL_CreateGPUComputePipeline");
    }
    return GpuComputePipeline(device, pipeline);
}

} // namespace core::sdl
