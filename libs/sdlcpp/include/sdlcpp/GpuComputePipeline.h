#pragma once

#include "sdlcpp/GpuHandle.h"

namespace core::sdl
{

using GpuComputePipeline = GpuHandle<SDL_GPUComputePipeline, SDL_ReleaseGPUComputePipeline>;

[[nodiscard]] GpuComputePipeline make_gpu_compute_pipeline(
    SDL_GPUDevice *device, const SDL_GPUComputePipelineCreateInfo &create_info);

} // namespace core::sdl
