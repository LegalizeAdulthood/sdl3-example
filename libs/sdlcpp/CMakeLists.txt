find_package(SDL3 CONFIG REQUIRED)

add_library(sdlcpp
    include/sdlcpp/Error.h
    include/sdlcpp/GpuBuffer.h
    include/sdlcpp/GpuCommandBuffer.h
    include/sdlcpp/GpuComputePass.h
    include/sdlcpp/GpuComputePipeline.h
    include/sdlcpp/GpuCopyPass.h
    include/sdlcpp/GpuDevice.h
    include/sdlcpp/GpuFence.h
    include/sdlcpp/GpuGraphicsPipeline.h
    include/sdlcpp/GpuHandle.h
    include/sdlcpp/GpuRenderPass.h
    include/sdlcpp/GpuSampler.h
    include/sdlcpp/GpuShader.h
    include/sdlcpp/GpuSwapchainTexture.h
    include/sdlcpp/GpuTexture.h
    include/sdlcpp/GpuTransferBuffer.h
    include/sdlcpp/GpuWindowClaim.h
    include/sdlcpp/Handle.h
    include/sdlcpp/Library.h
    include/sdlcpp/Properties.h
    include/sdlcpp/Window.h
    include/sdlcpp/sdl.h
    GpuBuffer.cpp
    GpuCommandBuffer.cpp
    GpuComputePass.cpp
    GpuComputePipeline.cpp
    GpuCopyPass.cpp
    GpuDevice.cpp
    GpuGraphicsPipeline.cpp
    GpuRenderPass.cpp
    GpuSampler.cpp
    GpuShader.cpp
    GpuSwapchainTexture.cpp
    GpuTexture.cpp
    GpuTransferBuffer.cpp
    GpuWindowClaim.cpp
    Library.cpp
    Properties.cpp
    Window.cpp
    sdl.cpp
)
target_include_directories(sdlcpp PUBLIC include)
target_link_libraries(sdlcpp PUBLIC SDL3::SDL3)
target_folder(sdlcpp "Libraries")
