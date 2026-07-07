#include <sdlcpp/GpuComputePass.h>

#include <sdlcpp/GpuComputePipeline.h>
#include <sdlcpp/sdl.h>

#include <utility>

namespace sdlcpp
{

GpuComputePass::GpuComputePass(SDL_GPUComputePass *compute_pass) noexcept :
    compute_pass_(compute_pass)
{
}

GpuComputePass::~GpuComputePass()
{
    EndGPUComputePass();
}

GpuComputePass::GpuComputePass(GpuComputePass &&other) noexcept :
    compute_pass_(std::exchange(other.compute_pass_, nullptr))
{
}

GpuComputePass &GpuComputePass::operator=(GpuComputePass &&other) noexcept
{
    if (this != &other)
    {
        EndGPUComputePass();
        compute_pass_ = std::exchange(other.compute_pass_, nullptr);
    }
    return *this;
}

void GpuComputePass::EndGPUComputePass() noexcept
{
    if (compute_pass_ != nullptr)
    {
        SDL_EndGPUComputePass(compute_pass_);
        compute_pass_ = nullptr;
    }
}

void GpuComputePass::BindGPUComputePipeline(const GpuComputePipeline &compute_pipeline)
{
    require_pointer(compute_pass_, "compute_pass");
    require_pointer(compute_pipeline.get(), "compute_pipeline");
    SDL_BindGPUComputePipeline(compute_pass_, compute_pipeline.get());
}

void GpuComputePass::DispatchGPUCompute(Uint32 groupcount_x, Uint32 groupcount_y, Uint32 groupcount_z)
{
    require_pointer(compute_pass_, "compute_pass");
    SDL_DispatchGPUCompute(compute_pass_, groupcount_x, groupcount_y, groupcount_z);
}

} // namespace sdlcpp
