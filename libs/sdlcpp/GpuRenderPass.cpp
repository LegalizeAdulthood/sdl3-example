#include <sdlcpp/GpuRenderPass.h>

#include <sdlcpp/GpuBuffer.h>
#include <sdlcpp/GpuGraphicsPipeline.h>
#include <sdlcpp/GpuTexture.h>
#include <sdlcpp/sdl.h>

#include <utility>

namespace sdlcpp
{

GpuRenderPass::GpuRenderPass(SDL_GPURenderPass *render_pass) noexcept :
    render_pass_(render_pass)
{
}

GpuRenderPass::~GpuRenderPass()
{
    EndGPURenderPass();
}

GpuRenderPass::GpuRenderPass(GpuRenderPass &&other) noexcept :
    render_pass_(std::exchange(other.render_pass_, nullptr))
{
}

GpuRenderPass &GpuRenderPass::operator=(GpuRenderPass &&other) noexcept
{
    if (this != &other)
    {
        EndGPURenderPass();
        render_pass_ = std::exchange(other.render_pass_, nullptr);
    }
    return *this;
}

void GpuRenderPass::EndGPURenderPass() noexcept
{
    if (render_pass_ != nullptr)
    {
        SDL_EndGPURenderPass(render_pass_);
        render_pass_ = nullptr;
    }
}

void GpuRenderPass::BindGPUGraphicsPipeline(const GpuGraphicsPipeline &graphics_pipeline)
{
    require_pointer(render_pass_, "render_pass");
    require_pointer(graphics_pipeline.get(), "graphics_pipeline");
    SDL_BindGPUGraphicsPipeline(render_pass_, graphics_pipeline.get());
}

void GpuRenderPass::BindGPUVertexStorageTextures(
    Uint32 first_slot, SDL_GPUTexture *const *storage_textures, Uint32 num_bindings)
{
    require_pointer(render_pass_, "render_pass");
    require_pointer(storage_textures, "storage_textures");
    SDL_BindGPUVertexStorageTextures(render_pass_, first_slot, storage_textures, num_bindings);
}

void GpuRenderPass::BindGPUVertexStorageTextures(Uint32 first_slot, const GpuTexture &texture)
{
    auto *storage_texture = texture.get();
    require_pointer(storage_texture, "texture");
    BindGPUVertexStorageTextures(first_slot, &storage_texture, 1);
}

void GpuRenderPass::BindGPUFragmentStorageTextures(
    Uint32 first_slot, SDL_GPUTexture *const *storage_textures, Uint32 num_bindings)
{
    require_pointer(render_pass_, "render_pass");
    require_pointer(storage_textures, "storage_textures");
    SDL_BindGPUFragmentStorageTextures(render_pass_, first_slot, storage_textures, num_bindings);
}

void GpuRenderPass::BindGPUFragmentStorageTextures(Uint32 first_slot, const GpuTexture &texture)
{
    auto *storage_texture = texture.get();
    require_pointer(storage_texture, "texture");
    BindGPUFragmentStorageTextures(first_slot, &storage_texture, 1);
}

void GpuRenderPass::BindGPUFragmentStorageBuffers(
    Uint32 first_slot, SDL_GPUBuffer *const *storage_buffers, Uint32 num_bindings)
{
    require_pointer(render_pass_, "render_pass");
    require_pointer(storage_buffers, "storage_buffers");
    SDL_BindGPUFragmentStorageBuffers(render_pass_, first_slot, storage_buffers, num_bindings);
}

void GpuRenderPass::BindGPUFragmentStorageBuffers(Uint32 first_slot, const GpuBuffer &buffer)
{
    auto *storage_buffer = buffer.get();
    require_pointer(storage_buffer, "buffer");
    BindGPUFragmentStorageBuffers(first_slot, &storage_buffer, 1);
}

void GpuRenderPass::DrawGPUPrimitives(
    Uint32 num_vertices, Uint32 num_instances, Uint32 first_vertex, Uint32 first_instance)
{
    require_pointer(render_pass_, "render_pass");
    SDL_DrawGPUPrimitives(render_pass_, num_vertices, num_instances, first_vertex, first_instance);
}

} // namespace sdlcpp
