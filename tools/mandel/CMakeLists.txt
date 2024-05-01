find_package(SDL3 CONFIG REQUIRED)
find_package(wxWidgets CONFIG REQUIRED)

add_executable(sdl3-mandel main.cpp)
target_link_libraries(sdl3-mandel PUBLIC core SDL3::SDL3 wx::core wx::base)
target_folder(sdl3-mandel "Tools")
