find_package(SDL3 CONFIG REQUIRED)
find_package(wxWidgets CONFIG REQUIRED)

add_executable(mandel main.cpp)
set_target_properties(mandel PROPERTIES WIN32_EXECUTABLE "$<PLATFORM_ID:Windows>")
target_link_libraries(mandel PUBLIC core wxsdl)
target_folder(mandel "Tools")
