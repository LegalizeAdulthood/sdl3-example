#pragma once

#include "sdlcpp/Handle.h"

#include <SDL3/SDL_gpu.h>

namespace core::sdl
{

using GpuDevice = Handle<SDL_GPUDevice, SDL_DestroyGPUDevice>;

[[nodiscard]] GpuDevice make_gpu_device(
    SDL_GPUShaderFormat format_flags, bool debug_mode = false, const char *name = nullptr);

} // namespace core::sdl
