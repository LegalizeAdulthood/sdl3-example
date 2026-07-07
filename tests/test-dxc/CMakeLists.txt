find_package(directx-dxc CONFIG REQUIRED)

set(dxc_smoke_source "${CMAKE_CURRENT_SOURCE_DIR}/smoke.vert.hlsl")
set(dxc_smoke_output "${CMAKE_CURRENT_BINARY_DIR}/smoke.vert.spv")

add_custom_command(
    OUTPUT "${dxc_smoke_output}"
    COMMAND
        "${DIRECTX_DXC_TOOL}"
        -spirv
        -T vs_6_0
        -E main
        -Fo "${dxc_smoke_output}"
        "${dxc_smoke_source}"
    DEPENDS "${dxc_smoke_source}"
    COMMENT "Compiling DXC smoke shader"
    VERBATIM)

add_custom_target(test-dxc DEPENDS "${dxc_smoke_output}")
target_folder(test-dxc "Tests")

add_test(
    NAME DxcSmoke.compile_hlsl
    COMMAND
        "${CMAKE_COMMAND}"
        --build "${CMAKE_BINARY_DIR}"
        --target test-dxc
        --config "$<CONFIG>")

add_test(
    NAME MandelShaders.compile_assets
    COMMAND
        "${CMAKE_COMMAND}"
        --build "${CMAKE_BINARY_DIR}"
        --target mandel-shaders
        --config "$<CONFIG>")
