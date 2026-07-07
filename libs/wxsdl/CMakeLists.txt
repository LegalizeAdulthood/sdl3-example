find_package(SDL3 CONFIG REQUIRED)
find_package(wxWidgets CONFIG REQUIRED)

add_library(wxsdl
    include/wxsdl/SdlCanvas.h
    SdlCanvas.cpp
    SdlWindowAttach.h
)

if(WIN32)
    target_sources(wxsdl PRIVATE SdlWindowAttachWin32.cpp)
elseif(APPLE)
    target_sources(wxsdl PRIVATE SdlWindowAttachCocoa.cpp)
elseif(UNIX)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTK3 REQUIRED IMPORTED_TARGET gtk+-3.0 gdk-x11-3.0)
    target_sources(wxsdl PRIVATE SdlWindowAttachGtk3.cpp)
    target_link_libraries(wxsdl PRIVATE PkgConfig::GTK3)
endif()

target_include_directories(wxsdl PUBLIC include)
target_link_libraries(wxsdl PUBLIC sdlcpp wx::core wx::base)
target_folder(wxsdl "Libraries")
