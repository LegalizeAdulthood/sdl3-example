find_package(SDL3 CONFIG REQUIRED)
find_package(SDL3_shadercross CONFIG REQUIRED)
find_package(wxWidgets CONFIG REQUIRED)

set(MANDEL_ASSETS_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/assets")
set(MANDEL_ASSETS_DIR "${CMAKE_CURRENT_BINARY_DIR}/assets")
set(MANDEL_SHADERCROSS_TOOL_DIR
    "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/tools/sdl3-shadercross")
find_program(
    SDL3_SHADERCROSS_TOOL
    NAMES shadercross shadercross.exe
    PATHS "${MANDEL_SHADERCROSS_TOOL_DIR}"
    REQUIRED)
file(MAKE_DIRECTORY "${MANDEL_ASSETS_DIR}")
configure_file("${MANDEL_ASSETS_SOURCE_DIR}/chroma.map" "${MANDEL_ASSETS_DIR}/chroma.map" COPYONLY)
configure_file(assets.h.in assets.h @ONLY)
set_source_files_properties(
    "${MANDEL_ASSETS_SOURCE_DIR}/blit.frag.hlsl"
    "${MANDEL_ASSETS_SOURCE_DIR}/blit.vert.hlsl"
    "${MANDEL_ASSETS_SOURCE_DIR}/height.frag.hlsl"
    "${MANDEL_ASSETS_SOURCE_DIR}/height.vert.hlsl"
    "${MANDEL_ASSETS_SOURCE_DIR}/mandel.comp.hlsl"
    PROPERTIES HEADER_FILE_ONLY ON)
source_group(
    "Shader Files"
    REGULAR_EXPRESSION "\\.hlsl$")

function(add_mandel_shader_assets output_var shader_source shader_name shader_stage)
    set(shader_outputs)
    foreach(shader_dest IN ITEMS DXIL SPIRV MSL)
        if(shader_dest STREQUAL "SPIRV")
            set(shader_ext "spv")
        else()
            string(TOLOWER "${shader_dest}" shader_ext)
        endif()
        set(shader_output "${MANDEL_ASSETS_DIR}/${shader_name}.${shader_ext}")
        add_custom_command(
            OUTPUT "${shader_output}"
            COMMAND
                "${SDL3_SHADERCROSS_TOOL}"
                "${shader_source}"
                --source HLSL
                --dest "${shader_dest}"
                --stage "${shader_stage}"
                --entrypoint main
                --output "${shader_output}"
            DEPENDS "${shader_source}"
            COMMENT "Compiling ${shader_name}.${shader_ext}"
            VERBATIM)
        list(APPEND shader_outputs "${shader_output}")
    endforeach()
    set(${output_var} ${shader_outputs} PARENT_SCOPE)
endfunction()

add_mandel_shader_assets(
    blit_vertex_shader_assets
    "${MANDEL_ASSETS_SOURCE_DIR}/blit.vert.hlsl"
    blit.vert
    vertex)
add_mandel_shader_assets(
    blit_fragment_shader_assets
    "${MANDEL_ASSETS_SOURCE_DIR}/blit.frag.hlsl"
    blit.frag
    fragment)
add_mandel_shader_assets(
    height_vertex_shader_assets
    "${MANDEL_ASSETS_SOURCE_DIR}/height.vert.hlsl"
    height.vert
    vertex)
add_mandel_shader_assets(
    height_fragment_shader_assets
    "${MANDEL_ASSETS_SOURCE_DIR}/height.frag.hlsl"
    height.frag
    fragment)
add_mandel_shader_assets(
    mandel_compute_shader_assets
    "${MANDEL_ASSETS_SOURCE_DIR}/mandel.comp.hlsl"
    mandel.comp
    compute)

add_custom_target(
    mandel-shaders
    DEPENDS
        ${blit_vertex_shader_assets}
        ${blit_fragment_shader_assets}
        ${height_vertex_shader_assets}
        ${height_fragment_shader_assets}
        ${mandel_compute_shader_assets})
target_folder(mandel-shaders "Tools")

add_executable(mandel
    "${CMAKE_CURRENT_BINARY_DIR}/assets.h"
    "${MANDEL_ASSETS_SOURCE_DIR}/blit.frag.hlsl"
    "${MANDEL_ASSETS_SOURCE_DIR}/blit.vert.hlsl"
    "${MANDEL_ASSETS_SOURCE_DIR}/height.frag.hlsl"
    "${MANDEL_ASSETS_SOURCE_DIR}/height.vert.hlsl"
    "${MANDEL_ASSETS_SOURCE_DIR}/mandel.comp.hlsl"
    MandelRenderHost.cpp
    MandelRenderHost.h
    assets.h.in
    main.cpp
)
add_dependencies(mandel mandel-shaders)
set_target_properties(mandel PROPERTIES WIN32_EXECUTABLE "$<PLATFORM_ID:Windows>")
target_include_directories(mandel PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
target_link_libraries(mandel PUBLIC core wxsdl)
target_folder(mandel "Tools")
