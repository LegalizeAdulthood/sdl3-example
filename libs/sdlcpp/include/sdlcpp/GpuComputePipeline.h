#pragma once

#include "sdlcpp/GpuHandle.h"

namespace sdlcpp
{

using GpuComputePipeline = GpuHandle<SDL_GPUComputePipeline, SDL_ReleaseGPUComputePipeline>;

[[nodiscard]] GpuComputePipeline make_gpu_compute_pipeline(
    SDL_GPUDevice *device, const SDL_GPUComputePipelineCreateInfo &create_info);

} // namespace sdlcpp
