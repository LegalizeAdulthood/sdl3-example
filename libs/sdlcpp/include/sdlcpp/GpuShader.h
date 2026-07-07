#pragma once

#include "sdlcpp/GpuHandle.h"

namespace core::sdl
{

using GpuShader = GpuHandle<SDL_GPUShader, SDL_ReleaseGPUShader>;

[[nodiscard]] GpuShader make_gpu_shader(SDL_GPUDevice *device, const SDL_GPUShaderCreateInfo &create_info);

} // namespace core::sdl
