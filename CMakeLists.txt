cmake_minimum_required(VERSION 3.16...4.0)
project(GramLib LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

add_library(GramLib INTERFACE)
target_include_directories(GramLib INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")