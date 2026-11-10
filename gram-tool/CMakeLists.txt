cmake_minimum_required(VERSION 3.16...4.0)
project(GramTool LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_BUILD_TYPE Release)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
    "${CMAKE_CURRENT_SOURCE_DIR}/dest"
)

add_subdirectory(.. gram-lib)

file(GLOB source CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

add_executable(GramTool ${source})

target_link_options(GramTool PRIVATE "-static")

target_link_libraries(GramTool PRIVATE
    GramLib
)

set_target_properties(GramTool PROPERTIES
    OUTPUT_NAME "gramtool"
)

add_compile_options(
    -Wall
    -Wextra
    -Wpedantic
    -Wconversion
    -Wsign-conversion
    -Wfloat-equal
    -Wformat=2
    -Wformat-security
    -Wnull-dereference
    -Wshadow
    -Wpointer-arith
    -Wcast-align
    -Wmissing-declarations
    -Woverloaded-virtual
    -Wnon-virtual-dtor
    -Wunused
    -Wuninitialized
)