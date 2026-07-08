#include <sdlcpp/GpuBuffer.h>
#include <sdlcpp/GpuCommandBuffer.h>
#include <sdlcpp/GpuGraphicsPipeline.h>
#include <sdlcpp/GpuRenderPass.h>
#include <sdlcpp/GpuTexture.h>

#include <gtest/gtest.h>

#include <type_traits>

namespace
{

TEST(GpuRenderPass, exposesSdlStyleMethods)
{
    using EndMethod = void (sdlcpp::GpuRenderPass::*)() noexcept;
    using BindPipelineMethod = void (sdlcpp::GpuRenderPass::*)(const sdlcpp::GpuGraphicsPipeline &);
    using BindTextureArrayMethod = void (sdlcpp::GpuRenderPass::*)(Uint32, SDL_GPUTexture *const *, Uint32);
    using BindTextureMethod = void (sdlcpp::GpuRenderPass::*)(Uint32, const sdlcpp::GpuTexture &);
    using BindBufferArrayMethod = void (sdlcpp::GpuRenderPass::*)(Uint32, SDL_GPUBuffer *const *, Uint32);
    using BindBufferMethod = void (sdlcpp::GpuRenderPass::*)(Uint32, const sdlcpp::GpuBuffer &);
    using DrawMethod = void (sdlcpp::GpuRenderPass::*)(Uint32, Uint32, Uint32, Uint32);

    static_assert(std::is_same_v<decltype(&sdlcpp::GpuRenderPass::EndGPURenderPass), EndMethod>);
    static_assert(std::is_same_v<decltype(&sdlcpp::GpuRenderPass::BindGPUGraphicsPipeline), BindPipelineMethod>);
    static_assert(std::is_same_v<decltype(static_cast<BindTextureArrayMethod>(
                                     &sdlcpp::GpuRenderPass::BindGPUVertexStorageTextures)),
        BindTextureArrayMethod>);
    static_assert(
        std::is_same_v<decltype(static_cast<BindTextureMethod>(&sdlcpp::GpuRenderPass::BindGPUVertexStorageTextures)),
            BindTextureMethod>);
    static_assert(std::is_same_v<decltype(static_cast<BindTextureArrayMethod>(
                                     &sdlcpp::GpuRenderPass::BindGPUFragmentStorageTextures)),
        BindTextureArrayMethod>);
    static_assert(
        std::is_same_v<decltype(static_cast<BindTextureMethod>(&sdlcpp::GpuRenderPass::BindGPUFragmentStorageTextures)),
            BindTextureMethod>);
    static_assert(std::is_same_v<decltype(static_cast<BindBufferArrayMethod>(
                                     &sdlcpp::GpuRenderPass::BindGPUFragmentStorageBuffers)),
        BindBufferArrayMethod>);
    static_assert(
        std::is_same_v<decltype(static_cast<BindBufferMethod>(&sdlcpp::GpuRenderPass::BindGPUFragmentStorageBuffers)),
            BindBufferMethod>);
    static_assert(std::is_same_v<decltype(&sdlcpp::GpuRenderPass::DrawGPUPrimitives), DrawMethod>);
}

} // namespace
