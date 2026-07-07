#include <sdlcpp/GpuCommandBuffer.h>
#include <sdlcpp/GpuComputePass.h>
#include <sdlcpp/GpuComputePipeline.h>

#include <gtest/gtest.h>

#include <type_traits>

namespace
{

TEST(GpuComputePass, exposesSdlStyleMethods)
{
    using EndMethod = void (sdlcpp::GpuComputePass::*)() noexcept;
    using BindMethod = void (sdlcpp::GpuComputePass::*)(const sdlcpp::GpuComputePipeline &);
    using DispatchMethod = void (sdlcpp::GpuComputePass::*)(Uint32, Uint32, Uint32);

    static_assert(std::is_same_v<decltype(&sdlcpp::GpuComputePass::EndGPUComputePass), EndMethod>);
    static_assert(std::is_same_v<decltype(&sdlcpp::GpuComputePass::BindGPUComputePipeline), BindMethod>);
    static_assert(std::is_same_v<decltype(&sdlcpp::GpuComputePass::DispatchGPUCompute), DispatchMethod>);
}

} // namespace
