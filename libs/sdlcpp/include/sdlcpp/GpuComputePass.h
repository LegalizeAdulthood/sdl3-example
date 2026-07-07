#pragma once

#include "sdlcpp/GpuComputePipeline.h"

#include <SDL3/SDL_gpu.h>

namespace sdlcpp
{

class GpuCommandBuffer;

class GpuComputePass
{
public:
    GpuComputePass() noexcept = default;
    explicit GpuComputePass(SDL_GPUComputePass *compute_pass) noexcept;
    ~GpuComputePass();

    GpuComputePass(const GpuComputePass &) = delete;
    GpuComputePass &operator=(const GpuComputePass &) = delete;

    GpuComputePass(GpuComputePass &&other) noexcept;
    GpuComputePass &operator=(GpuComputePass &&other) noexcept;

    void EndGPUComputePass() noexcept;
    void BindGPUComputePipeline(const GpuComputePipeline &compute_pipeline);
    void DispatchGPUCompute(Uint32 groupcount_x, Uint32 groupcount_y, Uint32 groupcount_z);

    [[nodiscard]] SDL_GPUComputePass *get() const noexcept
    {
        return compute_pass_;
    }

    explicit operator bool() const noexcept
    {
        return compute_pass_ != nullptr;
    }

private:
    SDL_GPUComputePass *compute_pass_ = nullptr;
};

} // namespace sdlcpp
