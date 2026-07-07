#pragma once

#include "sdlcpp/GpuHandle.h"

namespace sdlcpp
{

using GpuFence = GpuHandle<SDL_GPUFence, SDL_ReleaseGPUFence>;

} // namespace sdlcpp
