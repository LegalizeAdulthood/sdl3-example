include(GoogleTest)

find_package(GTest CONFIG REQUIRED)

add_executable(test-core test.cpp)
target_link_libraries(test-core PUBLIC core GTest::gtest_main)
target_folder(test-core "Tests")

gtest_discover_tests(test-core)
