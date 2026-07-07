#pragma once

#include "sdlcpp/GpuHandle.h"

namespace sdlcpp
{

using GpuTexture = GpuHandle<SDL_GPUTexture, SDL_ReleaseGPUTexture>;

[[nodiscard]] GpuTexture make_gpu_texture(SDL_GPUDevice *device, const SDL_GPUTextureCreateInfo &create_info);

} // namespace sdlcpp
