find_package(Git REQUIRED)

include(FetchContent)
FetchContent_Declare(
  doctest
  GIT_REPOSITORY https://github.com/onqtam/doctest.git
  GIT_TAG 2.3.7)

FetchContent_GetProperties(doctest)
if(NOT doctest_POPULATED)
  FetchContent_Populate(doctest)
  add_subdirectory(${doctest_SOURCE_DIR} ${doctest_BINARY_DIR})
endif()

set(DOCTEST_INCLUDE_DIR
    ${doctest_SOURCE_DIR}/doctest
    CACHE INTERNAL "Path to include folder for doctest")

include(${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake)
