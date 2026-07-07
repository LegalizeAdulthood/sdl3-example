#include <core/sdl/GpuRenderPass.h>

#include <core/sdl/sdl.h>

#include <utility>

namespace core::sdl
{

GpuRenderPass::GpuRenderPass(SDL_GPUCommandBuffer *command_buffer, const SDL_GPUColorTargetInfo *color_target_infos,
    Uint32 num_color_targets, const SDL_GPUDepthStencilTargetInfo *depth_stencil_target_info)
{
    begin(command_buffer, color_target_infos, num_color_targets, depth_stencil_target_info);
}

GpuRenderPass::~GpuRenderPass()
{
    end();
}

GpuRenderPass::GpuRenderPass(GpuRenderPass &&other) noexcept :
    render_pass_(std::exchange(other.render_pass_, nullptr))
{
}

GpuRenderPass &GpuRenderPass::operator=(GpuRenderPass &&other) noexcept
{
    if (this != &other)
    {
        end();
        render_pass_ = std::exchange(other.render_pass_, nullptr);
    }
    return *this;
}

void GpuRenderPass::begin(SDL_GPUCommandBuffer *command_buffer, const SDL_GPUColorTargetInfo *color_target_infos,
    Uint32 num_color_targets, const SDL_GPUDepthStencilTargetInfo *depth_stencil_target_info)
{
    require_pointer(command_buffer, "command_buffer");
    require_pointer(color_target_infos, "color_target_infos");
    end();
    render_pass_ =
        SDL_BeginGPURenderPass(command_buffer, color_target_infos, num_color_targets, depth_stencil_target_info);
    if (render_pass_ == nullptr)
    {
        throw_error("SDL_BeginGPURenderPass");
    }
}

void GpuRenderPass::end() noexcept
{
    if (render_pass_ != nullptr)
    {
        SDL_EndGPURenderPass(render_pass_);
        render_pass_ = nullptr;
    }
}

} // namespace core::sdl
