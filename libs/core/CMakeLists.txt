find_package(SDL3 CONFIG REQUIRED)

add_library(core
    include/core/core.h
    core.cpp
)
target_include_directories(core PUBLIC include)
target_folder(core "Libraries")
