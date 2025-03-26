cmake_minimum_required(VERSION 3.18)

project(
    zephyr
    VERSION 0.1.0
    LANGUAGES C CXX
)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(ColorDiagnostics)
include(TargetAddExtraWarnings)

if(WIN32)
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "Link to libs dynamically")
else()
    set(BUILD_SHARED_LIBS ON CACHE BOOL "Link to libs dynamically")
endif()

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/engine/libs/CMakeLists.txt")
    include(FetchSubmodules)
    fetch_submodules()
endif()

add_subdirectory(engine/libs)
add_subdirectory(engine/src)