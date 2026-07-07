#include <sdlcpp/GpuDevice.h>

#include <sdlcpp/sdl.h>

namespace sdlcpp
{

GpuDevice::GpuDevice(SDL_GPUDevice *device) noexcept :
    Base(device)
{
}

SDL_GPUShaderFormat GpuDevice::GetGPUShaderFormats() const
{
    require_pointer(get(), "device");
    return SDL_GetGPUShaderFormats(get());
}

SDL_GPUTextureFormat GpuDevice::GetGPUSwapchainTextureFormat(SDL_Window *window) const
{
    require_pointer(get(), "device");
    require_pointer(window, "window");
    return SDL_GetGPUSwapchainTextureFormat(get(), window);
}

GpuBuffer GpuDevice::CreateGPUBuffer(const SDL_GPUBufferCreateInfo &create_info) const
{
    return make_gpu_buffer(get(), create_info);
}

GpuComputePipeline GpuDevice::CreateGPUComputePipeline(const SDL_GPUComputePipelineCreateInfo &create_info) const
{
    return make_gpu_compute_pipeline(get(), create_info);
}

GpuGraphicsPipeline GpuDevice::CreateGPUGraphicsPipeline(const SDL_GPUGraphicsPipelineCreateInfo &create_info) const
{
    return make_gpu_graphics_pipeline(get(), create_info);
}

GpuShader GpuDevice::CreateGPUShader(const SDL_GPUShaderCreateInfo &create_info) const
{
    return make_gpu_shader(get(), create_info);
}

GpuTexture GpuDevice::CreateGPUTexture(const SDL_GPUTextureCreateInfo &create_info) const
{
    return make_gpu_texture(get(), create_info);
}

GpuTransferBuffer GpuDevice::CreateGPUTransferBuffer(const SDL_GPUTransferBufferCreateInfo &create_info) const
{
    return make_gpu_transfer_buffer(get(), create_info);
}

void *GpuDevice::MapGPUTransferBuffer(const GpuTransferBuffer &transfer_buffer, bool cycle) const
{
    require_pointer(get(), "device");
    require_pointer(transfer_buffer.get(), "transfer_buffer");
    void *mapped = SDL_MapGPUTransferBuffer(get(), transfer_buffer.get(), cycle);
    if (mapped == nullptr)
    {
        throw_error("SDL_MapGPUTransferBuffer");
    }
    return mapped;
}

void GpuDevice::UnmapGPUTransferBuffer(const GpuTransferBuffer &transfer_buffer) const
{
    require_pointer(get(), "device");
    require_pointer(transfer_buffer.get(), "transfer_buffer");
    SDL_UnmapGPUTransferBuffer(get(), transfer_buffer.get());
}

GpuCommandBuffer GpuDevice::AcquireGPUCommandBuffer() const
{
    return acquire_gpu_command_buffer(get());
}

GpuWindowClaim GpuDevice::ClaimWindowForGPUDevice(SDL_Window *window) const
{
    return claim_window_for_gpu_device(get(), window);
}

GpuDevice make_gpu_device(SDL_GPUShaderFormat format_flags, bool debug_mode, const char *name)
{
    SDL_GPUDevice *device = SDL_CreateGPUDevice(format_flags, debug_mode, name);
    if (device == nullptr)
    {
        throw_error("SDL_CreateGPUDevice");
    }
    return GpuDevice(device);
}

} // namespace sdlcpp
