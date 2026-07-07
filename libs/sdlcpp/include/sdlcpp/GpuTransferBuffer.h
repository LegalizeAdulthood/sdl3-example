#pragma once

#include "sdlcpp/GpuHandle.h"

namespace core::sdl
{

using GpuTransferBuffer = GpuHandle<SDL_GPUTransferBuffer, SDL_ReleaseGPUTransferBuffer>;

[[nodiscard]] GpuTransferBuffer make_gpu_transfer_buffer(
    SDL_GPUDevice *device, const SDL_GPUTransferBufferCreateInfo &create_info);

} // namespace core::sdl
