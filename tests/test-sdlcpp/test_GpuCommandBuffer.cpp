#include <sdlcpp/GpuCommandBuffer.h>
#include <sdlcpp/GpuComputePass.h>
#include <sdlcpp/GpuCopyPass.h>
#include <sdlcpp/GpuFence.h>
#include <sdlcpp/GpuRenderPass.h>
#include <sdlcpp/GpuSwapchainTexture.h>

#include <gtest/gtest.h>

#include <type_traits>

namespace
{

TEST(GpuCommandBuffer, exposesSdlStyleMethods)
{
    using SubmitMethod = void (sdlcpp::GpuCommandBuffer::*)();
    using SubmitFenceMethod = sdlcpp::GpuFence (sdlcpp::GpuCommandBuffer::*)();
    using BeginComputeMethod = sdlcpp::GpuComputePass (sdlcpp::GpuCommandBuffer::*)(
        const SDL_GPUStorageTextureReadWriteBinding *, Uint32, const SDL_GPUStorageBufferReadWriteBinding *, Uint32);
    using BeginCopyMethod = sdlcpp::GpuCopyPass (sdlcpp::GpuCommandBuffer::*)();
    using BeginRenderMethod = sdlcpp::GpuRenderPass (sdlcpp::GpuCommandBuffer::*)(
        const SDL_GPUColorTargetInfo *, Uint32, const SDL_GPUDepthStencilTargetInfo *);
    using UniformMethod = void (sdlcpp::GpuCommandBuffer::*)(Uint32, const void *, Uint32);
    using AcquireMethod = sdlcpp::GpuSwapchainTexture (sdlcpp::GpuCommandBuffer::*)(SDL_Window *);

    static_assert(std::is_same_v<decltype(&sdlcpp::GpuCommandBuffer::CancelGPUCommandBuffer), SubmitMethod>);
    static_assert(std::is_same_v<decltype(&sdlcpp::GpuCommandBuffer::SubmitGPUCommandBuffer), SubmitMethod>);
    static_assert(
        std::is_same_v<decltype(&sdlcpp::GpuCommandBuffer::SubmitGPUCommandBufferAndAcquireFence), SubmitFenceMethod>);
    static_assert(std::is_same_v<decltype(&sdlcpp::GpuCommandBuffer::BeginGPUComputePass), BeginComputeMethod>);
    static_assert(std::is_same_v<decltype(&sdlcpp::GpuCommandBuffer::BeginGPUCopyPass), BeginCopyMethod>);
    static_assert(std::is_same_v<decltype(&sdlcpp::GpuCommandBuffer::BeginGPURenderPass), BeginRenderMethod>);
    static_assert(std::is_same_v<decltype(&sdlcpp::GpuCommandBuffer::PushGPUVertexUniformData), UniformMethod>);
    static_assert(std::is_same_v<decltype(&sdlcpp::GpuCommandBuffer::PushGPUFragmentUniformData), UniformMethod>);
    static_assert(std::is_same_v<decltype(&sdlcpp::GpuCommandBuffer::PushGPUComputeUniformData), UniformMethod>);
    static_assert(std::is_same_v<decltype(&sdlcpp::GpuCommandBuffer::AcquireGPUSwapchainTexture), AcquireMethod>);
    static_assert(
        std::is_same_v<decltype(&sdlcpp::GpuCommandBuffer::WaitAndAcquireGPUSwapchainTexture), AcquireMethod>);
}

} // namespace
