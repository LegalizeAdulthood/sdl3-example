#pragma once

#include "sdlcpp/GpuHandle.h"

namespace core::sdl
{

using GpuBuffer = GpuHandle<SDL_GPUBuffer, SDL_ReleaseGPUBuffer>;

[[nodiscard]] GpuBuffer make_gpu_buffer(SDL_GPUDevice *device, const SDL_GPUBufferCreateInfo &create_info);

} // namespace core::sdl
