#pragma once

#include "core/sdl/GpuHandle.h"

namespace core::sdl
{

using GpuSampler = GpuHandle<SDL_GPUSampler, SDL_ReleaseGPUSampler>;

[[nodiscard]] GpuSampler make_gpu_sampler(SDL_GPUDevice *device, const SDL_GPUSamplerCreateInfo &create_info);

} // namespace core::sdl
