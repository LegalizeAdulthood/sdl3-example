#include <sdlcpp/GpuBuffer.h>
#include <sdlcpp/GpuCommandBuffer.h>
#include <sdlcpp/GpuCopyPass.h>
#include <sdlcpp/GpuTransferBuffer.h>

#include <gtest/gtest.h>

#include <type_traits>

namespace
{

TEST(GpuCopyPass, exposesSdlStyleMethods)
{
    using EndMethod = void (sdlcpp::GpuCopyPass::*)() noexcept;
    using RawUploadMethod =
        void (sdlcpp::GpuCopyPass::*)(const SDL_GPUTransferBufferLocation *, const SDL_GPUBufferRegion *, bool);
    using UploadMethod = void (sdlcpp::GpuCopyPass::*)(
        const sdlcpp::GpuTransferBuffer &, Uint32, const sdlcpp::GpuBuffer &, Uint32, Uint32, bool);

    static_assert(std::is_same_v<decltype(&sdlcpp::GpuCopyPass::EndGPUCopyPass), EndMethod>);
    static_assert(std::is_same_v<decltype(static_cast<RawUploadMethod>(&sdlcpp::GpuCopyPass::UploadToGPUBuffer)),
        RawUploadMethod>);
    static_assert(
        std::is_same_v<decltype(static_cast<UploadMethod>(&sdlcpp::GpuCopyPass::UploadToGPUBuffer)), UploadMethod>);
}

} // namespace
