cmake_minimum_required(VERSION 3.14)

project(
  Lox-modern-cpp
  VERSION 0.1
  DESCRIPTION "Lox modern cpp implement"
  LANGUAGES CXX)

enable_testing()

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
include(doctest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_FLAGS_RELEASE
    "${CMAKE_CXX_FLAGS_RELEASE} -DDOCTEST_CONFIG_DISABLE")

add_subdirectory(src)
