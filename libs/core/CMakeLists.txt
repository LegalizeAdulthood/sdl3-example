add_library(core
    include/core/MandelCpu.h
    include/core/MandelImage.h
    include/core/MandelIterationBuffer.h
    include/core/MandelParams.h
    include/core/MandelViewport.h
    MandelCpu.cpp
    MandelImage.cpp
    MandelViewport.cpp
)
target_include_directories(core PUBLIC include)
target_folder(core "Libraries")
