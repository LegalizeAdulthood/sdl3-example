include(GoogleTest)

find_package(GTest CONFIG REQUIRED)

add_executable(test-core
    test_MandelCpu.cpp
    test_MandelImage.cpp
    test_MandelIterationBuffer.cpp
    test_MandelPalette.cpp
)
target_link_libraries(test-core PUBLIC core GTest::gtest_main)
target_folder(test-core "Tests")

gtest_discover_tests(test-core)
