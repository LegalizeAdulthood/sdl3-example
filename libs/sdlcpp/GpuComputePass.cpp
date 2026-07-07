#include <sdlcpp/GpuComputePass.h>

#include <sdlcpp/sdl.h>

#include <utility>

namespace core::sdl
{

GpuComputePass::GpuComputePass(SDL_GPUCommandBuffer *command_buffer,
    const SDL_GPUStorageTextureReadWriteBinding *storage_texture_bindings, Uint32 num_storage_texture_bindings,
    const SDL_GPUStorageBufferReadWriteBinding *storage_buffer_bindings, Uint32 num_storage_buffer_bindings)
{
    begin(command_buffer, storage_texture_bindings, num_storage_texture_bindings, storage_buffer_bindings,
        num_storage_buffer_bindings);
}

GpuComputePass::~GpuComputePass()
{
    end();
}

GpuComputePass::GpuComputePass(GpuComputePass &&other) noexcept :
    compute_pass_(std::exchange(other.compute_pass_, nullptr))
{
}

GpuComputePass &GpuComputePass::operator=(GpuComputePass &&other) noexcept
{
    if (this != &other)
    {
        end();
        compute_pass_ = std::exchange(other.compute_pass_, nullptr);
    }
    return *this;
}

void GpuComputePass::begin(SDL_GPUCommandBuffer *command_buffer,
    const SDL_GPUStorageTextureReadWriteBinding *storage_texture_bindings, Uint32 num_storage_texture_bindings,
    const SDL_GPUStorageBufferReadWriteBinding *storage_buffer_bindings, Uint32 num_storage_buffer_bindings)
{
    require_pointer(command_buffer, "command_buffer");
    end();
    compute_pass_ = SDL_BeginGPUComputePass(command_buffer, storage_texture_bindings, num_storage_texture_bindings,
        storage_buffer_bindings, num_storage_buffer_bindings);
    if (compute_pass_ == nullptr)
    {
        throw_error("SDL_BeginGPUComputePass");
    }
}

void GpuComputePass::end() noexcept
{
    if (compute_pass_ != nullptr)
    {
        SDL_EndGPUComputePass(compute_pass_);
        compute_pass_ = nullptr;
    }
}

} // namespace core::sdl
