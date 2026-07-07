#pragma once

#include "sdlcpp/GpuHandle.h"

namespace sdlcpp
{

using GpuGraphicsPipeline = GpuHandle<SDL_GPUGraphicsPipeline, SDL_ReleaseGPUGraphicsPipeline>;

[[nodiscard]] GpuGraphicsPipeline make_gpu_graphics_pipeline(
    SDL_GPUDevice *device, const SDL_GPUGraphicsPipelineCreateInfo &create_info);

} // namespace sdlcpp
