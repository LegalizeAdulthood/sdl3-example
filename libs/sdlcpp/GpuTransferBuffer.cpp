#include <sdlcpp/GpuTransferBuffer.h>

#include <sdlcpp/sdl.h>

namespace sdlcpp
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

} // namespace sdlcpp
