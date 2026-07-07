#include <sdlcpp/GpuCopyPass.h>

#include <sdlcpp/GpuBuffer.h>
#include <sdlcpp/GpuTransferBuffer.h>
#include <sdlcpp/sdl.h>

#include <utility>

namespace sdlcpp
{

GpuCopyPass::GpuCopyPass(SDL_GPUCopyPass *copy_pass) noexcept :
    copy_pass_(copy_pass)
{
}

GpuCopyPass::~GpuCopyPass()
{
    EndGPUCopyPass();
}

GpuCopyPass::GpuCopyPass(GpuCopyPass &&other) noexcept :
    copy_pass_(std::exchange(other.copy_pass_, nullptr))
{
}

GpuCopyPass &GpuCopyPass::operator=(GpuCopyPass &&other) noexcept
{
    if (this != &other)
    {
        EndGPUCopyPass();
        copy_pass_ = std::exchange(other.copy_pass_, nullptr);
    }
    return *this;
}

void GpuCopyPass::EndGPUCopyPass() noexcept
{
    if (copy_pass_ != nullptr)
    {
        SDL_EndGPUCopyPass(copy_pass_);
        copy_pass_ = nullptr;
    }
}

void GpuCopyPass::UploadToGPUBuffer(
    const SDL_GPUTransferBufferLocation *source, const SDL_GPUBufferRegion *destination, bool cycle)
{
    require_pointer(copy_pass_, "copy_pass");
    require_pointer(source, "source");
    require_pointer(destination, "destination");
    SDL_UploadToGPUBuffer(copy_pass_, source, destination, cycle);
}

void GpuCopyPass::UploadToGPUBuffer(const GpuTransferBuffer &source_buffer, Uint32 source_offset,
    const GpuBuffer &destination_buffer, Uint32 destination_offset, Uint32 size, bool cycle)
{
    require_pointer(source_buffer.get(), "source_buffer");
    require_pointer(destination_buffer.get(), "destination_buffer");
    const SDL_GPUTransferBufferLocation source{source_buffer.get(), source_offset};
    const SDL_GPUBufferRegion destination{destination_buffer.get(), destination_offset, size};
    UploadToGPUBuffer(&source, &destination, cycle);
}

} // namespace sdlcpp
