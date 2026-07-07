#include <sdlcpp/GpuGraphicsPipeline.h>

#include <sdlcpp/sdl.h>

namespace sdlcpp
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

} // namespace sdlcpp
