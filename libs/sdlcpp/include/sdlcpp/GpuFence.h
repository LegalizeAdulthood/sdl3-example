#pragma once

#include "sdlcpp/GpuHandle.h"

namespace core::sdl
{

using GpuFence = GpuHandle<SDL_GPUFence, SDL_ReleaseGPUFence>;

} // namespace core::sdl
