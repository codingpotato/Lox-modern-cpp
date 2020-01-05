cmake_minimum_required(VERSION 3.12)

project(Lox-modern-cpp
        VERSION 0.1
        DESCRIPTION "Lox modern cpp implement"
        LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

enable_testing()
include(catch2)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(src)
add_subdirectory(tests)
