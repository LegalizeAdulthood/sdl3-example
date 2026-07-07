find_package(SDL3 CONFIG REQUIRED)
find_package(wxWidgets CONFIG REQUIRED)

set(MANDEL_ASSETS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/assets")
configure_file(assets.h.in assets.h @ONLY)

add_executable(mandel
    "${CMAKE_CURRENT_BINARY_DIR}/assets.h"
    MandelRenderHost.cpp
    MandelRenderHost.h
    assets.h.in
    main.cpp
)
set_target_properties(mandel PROPERTIES WIN32_EXECUTABLE "$<PLATFORM_ID:Windows>")
target_include_directories(mandel PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
target_link_libraries(mandel PUBLIC core wxsdl)
target_folder(mandel "Tools")
