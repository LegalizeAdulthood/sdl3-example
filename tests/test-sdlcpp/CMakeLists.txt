include(GoogleTest)

find_package(GTest CONFIG REQUIRED)

add_executable(
    test-sdlcpp
    test.cpp
    test_GpuCommandBuffer.cpp
    test_GpuComputePass.cpp
    test_GpuCopyPass.cpp
    test_GpuDevice.cpp
    test_GpuRenderPass.cpp
)
target_link_libraries(test-sdlcpp PUBLIC sdlcpp GTest::gtest_main)
target_folder(test-sdlcpp "Tests")

gtest_discover_tests(test-sdlcpp NO_PRETTY_TYPES)
