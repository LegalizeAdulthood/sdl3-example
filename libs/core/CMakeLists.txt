add_library(core
    include/core/MandelCpu.h
    include/core/MandelParams.h
    MandelCpu.cpp
)
target_include_directories(core PUBLIC include)
target_folder(core "Libraries")
