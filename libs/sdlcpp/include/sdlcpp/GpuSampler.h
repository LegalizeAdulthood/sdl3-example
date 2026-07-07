#pragma once

#include "sdlcpp/GpuHandle.h"

namespace sdlcpp
{

using GpuSampler = GpuHandle<SDL_GPUSampler, SDL_ReleaseGPUSampler>;

[[nodiscard]] GpuSampler make_gpu_sampler(SDL_GPUDevice *device, const SDL_GPUSamplerCreateInfo &create_info);

} // namespace sdlcpp
