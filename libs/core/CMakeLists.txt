find_package(SDL3 CONFIG REQUIRED)

add_library(core
    include/core/core.h
    include/core/sdl/Error.h
    include/core/sdl/GpuBuffer.h
    include/core/sdl/GpuCommandBuffer.h
    include/core/sdl/GpuComputePass.h
    include/core/sdl/GpuComputePipeline.h
    include/core/sdl/GpuCopyPass.h
    include/core/sdl/GpuDevice.h
    include/core/sdl/GpuFence.h
    include/core/sdl/GpuGraphicsPipeline.h
    include/core/sdl/GpuHandle.h
    include/core/sdl/GpuRenderPass.h
    include/core/sdl/GpuSampler.h
    include/core/sdl/GpuShader.h
    include/core/sdl/GpuSwapchainTexture.h
    include/core/sdl/GpuTexture.h
    include/core/sdl/GpuTransferBuffer.h
    include/core/sdl/GpuWindowClaim.h
    include/core/sdl/Handle.h
    include/core/sdl/Library.h
    include/core/sdl/Properties.h
    include/core/sdl/Window.h
    include/core/sdl/sdl.h
    core.cpp
    sdl/GpuBuffer.cpp
    sdl/GpuCommandBuffer.cpp
    sdl/GpuComputePass.cpp
    sdl/GpuComputePipeline.cpp
    sdl/GpuCopyPass.cpp
    sdl/GpuDevice.cpp
    sdl/GpuGraphicsPipeline.cpp
    sdl/GpuRenderPass.cpp
    sdl/GpuSampler.cpp
    sdl/GpuShader.cpp
    sdl/GpuSwapchainTexture.cpp
    sdl/GpuTexture.cpp
    sdl/GpuTransferBuffer.cpp
    sdl/GpuWindowClaim.cpp
    sdl/Library.cpp
    sdl/Properties.cpp
    sdl/Window.cpp
    sdl/sdl.cpp
)
target_include_directories(core PUBLIC include)
target_link_libraries(core PUBLIC SDL3::SDL3)
target_folder(core "Libraries")
