#include <sdlcpp/GpuBuffer.h>
#include <sdlcpp/GpuCommandBuffer.h>
#include <sdlcpp/GpuComputePipeline.h>
#include <sdlcpp/GpuDevice.h>
#include <sdlcpp/GpuGraphicsPipeline.h>
#include <sdlcpp/GpuShader.h>
#include <sdlcpp/GpuTexture.h>
#include <sdlcpp/GpuTransferBuffer.h>
#include <sdlcpp/GpuWindowClaim.h>

#include <gtest/gtest.h>

#include <type_traits>

namespace
{

TEST(GpuDevice, exposesSdlStyleMethods)
{
    using GetShaderFormatsMethod = SDL_GPUShaderFormat (sdlcpp::GpuDevice::*)() const;
    using GetSwapchainFormatMethod = SDL_GPUTextureFormat (sdlcpp::GpuDevice::*)(SDL_Window *) const;
    using CreateBufferMethod = sdlcpp::GpuBuffer (sdlcpp::GpuDevice::*)(const SDL_GPUBufferCreateInfo &) const;
    using CreateComputePipelineMethod =
        sdlcpp::GpuComputePipeline (sdlcpp::GpuDevice::*)(const SDL_GPUComputePipelineCreateInfo &) const;
    using CreateGraphicsPipelineMethod =
        sdlcpp::GpuGraphicsPipeline (sdlcpp::GpuDevice::*)(const SDL_GPUGraphicsPipelineCreateInfo &) const;
    using CreateShaderMethod = sdlcpp::GpuShader (sdlcpp::GpuDevice::*)(const SDL_GPUShaderCreateInfo &) const;
    using CreateTextureMethod = sdlcpp::GpuTexture (sdlcpp::GpuDevice::*)(const SDL_GPUTextureCreateInfo &) const;
    using CreateTransferBufferMethod =
        sdlcpp::GpuTransferBuffer (sdlcpp::GpuDevice::*)(const SDL_GPUTransferBufferCreateInfo &) const;
    using MapTransferBufferMethod = void *(sdlcpp::GpuDevice::*) (const sdlcpp::GpuTransferBuffer &, bool) const;
    using UnmapTransferBufferMethod = void (sdlcpp::GpuDevice::*)(const sdlcpp::GpuTransferBuffer &) const;
    using AcquireCommandBufferMethod = sdlcpp::GpuCommandBuffer (sdlcpp::GpuDevice::*)() const;
    using ClaimWindowMethod = sdlcpp::GpuWindowClaim (sdlcpp::GpuDevice::*)(SDL_Window *) const;

    static_assert(std::is_same_v<decltype(&sdlcpp::GpuDevice::GetGPUShaderFormats), GetShaderFormatsMethod>);
    static_assert(std::is_same_v<decltype(&sdlcpp::GpuDevice::GetGPUSwapchainTextureFormat), GetSwapchainFormatMethod>);
    static_assert(std::is_same_v<decltype(&sdlcpp::GpuDevice::CreateGPUBuffer), CreateBufferMethod>);
    static_assert(std::is_same_v<decltype(&sdlcpp::GpuDevice::CreateGPUComputePipeline), CreateComputePipelineMethod>);
    static_assert(
        std::is_same_v<decltype(&sdlcpp::GpuDevice::CreateGPUGraphicsPipeline), CreateGraphicsPipelineMethod>);
    static_assert(std::is_same_v<decltype(&sdlcpp::GpuDevice::CreateGPUShader), CreateShaderMethod>);
    static_assert(std::is_same_v<decltype(&sdlcpp::GpuDevice::CreateGPUTexture), CreateTextureMethod>);
    static_assert(std::is_same_v<decltype(&sdlcpp::GpuDevice::CreateGPUTransferBuffer), CreateTransferBufferMethod>);
    static_assert(std::is_same_v<decltype(&sdlcpp::GpuDevice::MapGPUTransferBuffer), MapTransferBufferMethod>);
    static_assert(std::is_same_v<decltype(&sdlcpp::GpuDevice::UnmapGPUTransferBuffer), UnmapTransferBufferMethod>);
    static_assert(std::is_same_v<decltype(&sdlcpp::GpuDevice::AcquireGPUCommandBuffer), AcquireCommandBufferMethod>);
    static_assert(std::is_same_v<decltype(&sdlcpp::GpuDevice::ClaimWindowForGPUDevice), ClaimWindowMethod>);
}

} // namespace
