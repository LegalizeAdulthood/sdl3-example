#include <core/sdl/GpuGraphicsPipeline.h>

#include <core/sdl/sdl.h>

namespace core::sdl
{

GpuGraphicsPipeline make_gpu_graphics_pipeline(
    SDL_GPUDevice *device, const SDL_GPUGraphicsPipelineCreateInfo &create_info)
{
    require_pointer(device, "device");
    SDL_GPUGraphicsPipeline *pipeline = SDL_CreateGPUGraphicsPipeline(device, &create_info);
    if (pipeline == nullptr)
    {
        throw_error("SDL_CreateGPUGraphicsPipeline");
    }
    return GpuGraphicsPipeline(device, pipeline);
}

} // namespace core::sdl
