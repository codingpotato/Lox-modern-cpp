cmake_minimum_required(VERSION 3.13)

project(
  Lox-modern-cpp
  VERSION 0.1
  DESCRIPTION "Lox modern cpp implement"
  LANGUAGES CXX)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
include(coverage)
include(update_submodules)

enable_testing()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_FLAGS_RELEASE
    "${CMAKE_CXX_FLAGS_RELEASE} -DDOCTEST_CONFIG_DISABLE")

set(BENCHMARK_ENABLE_TESTING
    OFF
    CACHE BOOL "Suppressing benchmark's tests" FORCE)

set(BENCHMARK_DIR ${PROJECT_SOURCE_DIR}/tools/benchmark)
set(DOCTEST_DIR ${PROJECT_SOURCE_DIR}/tools/doctest)
add_subdirectory(tools)

configure_file(config.h.in ${CMAKE_BINARY_DIR}/config.h @ONLY)

add_subdirectory(benchmark)
add_subdirectory(src)
add_subdirectory(tests)

add_executable(lox main.cpp ${TESTS_SOURCES})
add_dependencies(lox doctest)
target_link_libraries(lox PUBLIC lox_core)

include(${DOCTEST_DIR}/scripts/cmake/doctest.cmake)
doctest_discover_tests(lox)
