#pragma once

#include "sdlcpp/GpuHandle.h"

namespace sdlcpp
{

using GpuShader = GpuHandle<SDL_GPUShader, SDL_ReleaseGPUShader>;

[[nodiscard]] GpuShader make_gpu_shader(SDL_GPUDevice *device, const SDL_GPUShaderCreateInfo &create_info);

} // namespace sdlcpp
