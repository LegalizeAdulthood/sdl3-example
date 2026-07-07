#pragma once

#include "sdlcpp/GpuBuffer.h"
#include "sdlcpp/GpuCommandBuffer.h"
#include "sdlcpp/GpuComputePipeline.h"
#include "sdlcpp/GpuGraphicsPipeline.h"
#include "sdlcpp/GpuShader.h"
#include "sdlcpp/GpuTexture.h"
#include "sdlcpp/GpuTransferBuffer.h"
#include "sdlcpp/GpuWindowClaim.h"
#include "sdlcpp/Handle.h"

#include <SDL3/SDL_gpu.h>

namespace sdlcpp
{

class GpuDevice : public Handle<SDL_GPUDevice, SDL_DestroyGPUDevice>
{
public:
    using Base = Handle<SDL_GPUDevice, SDL_DestroyGPUDevice>;

    GpuDevice() noexcept = default;
    explicit GpuDevice(SDL_GPUDevice *device) noexcept;

    [[nodiscard]] SDL_GPUShaderFormat GetGPUShaderFormats() const;
    [[nodiscard]] SDL_GPUTextureFormat GetGPUSwapchainTextureFormat(SDL_Window *window) const;
    [[nodiscard]] GpuBuffer CreateGPUBuffer(const SDL_GPUBufferCreateInfo &create_info) const;
    [[nodiscard]] GpuComputePipeline CreateGPUComputePipeline(
        const SDL_GPUComputePipelineCreateInfo &create_info) const;
    [[nodiscard]] GpuGraphicsPipeline CreateGPUGraphicsPipeline(
        const SDL_GPUGraphicsPipelineCreateInfo &create_info) const;
    [[nodiscard]] GpuShader CreateGPUShader(const SDL_GPUShaderCreateInfo &create_info) const;
    [[nodiscard]] GpuTexture CreateGPUTexture(const SDL_GPUTextureCreateInfo &create_info) const;
    [[nodiscard]] GpuTransferBuffer CreateGPUTransferBuffer(const SDL_GPUTransferBufferCreateInfo &create_info) const;
    [[nodiscard]] void *MapGPUTransferBuffer(const GpuTransferBuffer &transfer_buffer, bool cycle) const;
    void UnmapGPUTransferBuffer(const GpuTransferBuffer &transfer_buffer) const;
    [[nodiscard]] GpuCommandBuffer AcquireGPUCommandBuffer() const;
    [[nodiscard]] GpuWindowClaim ClaimWindowForGPUDevice(SDL_Window *window) const;
};

[[nodiscard]] GpuDevice make_gpu_device(
    SDL_GPUShaderFormat format_flags, bool debug_mode = false, const char *name = nullptr);

} // namespace sdlcpp
