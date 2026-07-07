#pragma once

#include <SDL3/SDL_gpu.h>

namespace sdlcpp
{

class GpuRenderPass
{
public:
    GpuRenderPass() noexcept = default;
    GpuRenderPass(SDL_GPUCommandBuffer *command_buffer, const SDL_GPUColorTargetInfo *color_target_infos,
        Uint32 num_color_targets, const SDL_GPUDepthStencilTargetInfo *depth_stencil_target_info = nullptr);
    ~GpuRenderPass();

    GpuRenderPass(const GpuRenderPass &) = delete;
    GpuRenderPass &operator=(const GpuRenderPass &) = delete;

    GpuRenderPass(GpuRenderPass &&other) noexcept;
    GpuRenderPass &operator=(GpuRenderPass &&other) noexcept;

    void begin(SDL_GPUCommandBuffer *command_buffer, const SDL_GPUColorTargetInfo *color_target_infos,
        Uint32 num_color_targets, const SDL_GPUDepthStencilTargetInfo *depth_stencil_target_info = nullptr);
    void end() noexcept;

    [[nodiscard]] SDL_GPURenderPass *get() const noexcept
    {
        return render_pass_;
    }

    explicit operator bool() const noexcept
    {
        return render_pass_ != nullptr;
    }

private:
    SDL_GPURenderPass *render_pass_ = nullptr;
};

} // namespace sdlcpp
