#pragma once

#include "sdlcpp/GpuBuffer.h"
#include "sdlcpp/GpuGraphicsPipeline.h"
#include "sdlcpp/GpuTexture.h"

#include <SDL3/SDL_gpu.h>

namespace sdlcpp
{

class GpuCommandBuffer;

class GpuRenderPass
{
public:
    GpuRenderPass() noexcept = default;
    explicit GpuRenderPass(SDL_GPURenderPass *render_pass) noexcept;
    ~GpuRenderPass();

    GpuRenderPass(const GpuRenderPass &) = delete;
    GpuRenderPass &operator=(const GpuRenderPass &) = delete;

    GpuRenderPass(GpuRenderPass &&other) noexcept;
    GpuRenderPass &operator=(GpuRenderPass &&other) noexcept;

    void EndGPURenderPass() noexcept;
    void BindGPUGraphicsPipeline(const GpuGraphicsPipeline &graphics_pipeline);
    void BindGPUFragmentStorageTextures(
        Uint32 first_slot, SDL_GPUTexture *const *storage_textures, Uint32 num_bindings);
    void BindGPUFragmentStorageTextures(Uint32 first_slot, const GpuTexture &texture);
    void BindGPUFragmentStorageBuffers(Uint32 first_slot, SDL_GPUBuffer *const *storage_buffers, Uint32 num_bindings);
    void BindGPUFragmentStorageBuffers(Uint32 first_slot, const GpuBuffer &buffer);
    void DrawGPUPrimitives(Uint32 num_vertices, Uint32 num_instances, Uint32 first_vertex, Uint32 first_instance);

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
