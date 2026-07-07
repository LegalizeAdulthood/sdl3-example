#include <core/sdl/GpuCopyPass.h>

#include <core/sdl/sdl.h>

#include <utility>

namespace core::sdl
{

GpuCopyPass::GpuCopyPass(SDL_GPUCommandBuffer *command_buffer)
{
    begin(command_buffer);
}

GpuCopyPass::~GpuCopyPass()
{
    end();
}

GpuCopyPass::GpuCopyPass(GpuCopyPass &&other) noexcept :
    copy_pass_(std::exchange(other.copy_pass_, nullptr))
{
}

GpuCopyPass &GpuCopyPass::operator=(GpuCopyPass &&other) noexcept
{
    if (this != &other)
    {
        end();
        copy_pass_ = std::exchange(other.copy_pass_, nullptr);
    }
    return *this;
}

void GpuCopyPass::begin(SDL_GPUCommandBuffer *command_buffer)
{
    require_pointer(command_buffer, "command_buffer");
    end();
    copy_pass_ = SDL_BeginGPUCopyPass(command_buffer);
    if (copy_pass_ == nullptr)
    {
        throw_error("SDL_BeginGPUCopyPass");
    }
}

void GpuCopyPass::end() noexcept
{
    if (copy_pass_ != nullptr)
    {
        SDL_EndGPUCopyPass(copy_pass_);
        copy_pass_ = nullptr;
    }
}

} // namespace core::sdl
