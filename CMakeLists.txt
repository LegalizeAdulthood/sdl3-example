cmake_minimum_required(VERSION 3.23...3.31)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
include(misc_target)
include(set_cxx_standard)
include(target_folder)
include(vs_startup_project)

set_cxx_standard(17)

set(SDL3_EXAMPLE_PROJECT_NAME "SDL3 Example" CACHE "STRING" "Default project name for IDEs")
project("${SDL3_EXAMPLE_PROJECT_NAME}" CXX)

include(CTest)

add_subdirectory(libs)
add_subdirectory(tools)

vs_startup_project(sdl3-mandel)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

misc_target(FILES
    "CMakePresets.json"
    "ReadMe.md")
misc_group_sources("CMake Scripts" FILES
    "cmake/misc_target.cmake"
    "cmake/set_cxx_standard.cmake"
    "cmake/target_folder.cmake"
    "cmake/vs_startup_project.cmake")
misc_group_sources("Editing" FILES
    ".clang-format"
    ".editorconfig")
misc_group_sources("Git" FILES
    ".gitignore"
    ".gitmodules")
misc_group_sources("GitHub" FILES
    ".github/workflows/cmake.yml")
misc_group_sources("VcPkg" FILES
    "vcpkg-overlay-ports/sdl3-shadercross/fix-directx-shader-compiler-includes.patch"
    "vcpkg-overlay-ports/sdl3-shadercross/portfile.cmake"
    "vcpkg-overlay-ports/sdl3-shadercross/usage"
    "vcpkg-overlay-ports/sdl3-shadercross/vcpkg.json"
    "vcpkg-configuration.json"
    "vcpkg.json")
