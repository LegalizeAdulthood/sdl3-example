#pragma once

#include "core/sdl/GpuHandle.h"

namespace core::sdl
{

using GpuFence = GpuHandle<SDL_GPUFence, SDL_ReleaseGPUFence>;

} // namespace core::sdl
