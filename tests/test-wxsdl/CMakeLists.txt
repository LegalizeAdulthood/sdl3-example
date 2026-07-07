include(GoogleTest)

find_package(GTest CONFIG REQUIRED)

add_executable(test-wxsdl test.cpp)
target_link_libraries(test-wxsdl PUBLIC wxsdl GTest::gtest_main)
target_folder(test-wxsdl "Tests")

gtest_discover_tests(test-wxsdl NO_PRETTY_TYPES)
