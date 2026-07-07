#include <core/sdl/GpuTransferBuffer.h>

#include <core/sdl/sdl.h>

namespace core::sdl
{

GpuTransferBuffer make_gpu_transfer_buffer(SDL_GPUDevice *device, const SDL_GPUTransferBufferCreateInfo &create_info)
{
    require_pointer(device, "device");
    SDL_GPUTransferBuffer *buffer = SDL_CreateGPUTransferBuffer(device, &create_info);
    if (buffer == nullptr)
    {
        throw_error("SDL_CreateGPUTransferBuffer");
    }
    return GpuTransferBuffer(device, buffer);
}

} // namespace core::sdl
