file(
  GLOB
  GLOB_SOURCES
  chunk_tests.cpp
  compiler_test.cpp
  gc_tests.cpp
  hash_table_tests.cpp
  heap_tests.cpp
  lox_tests.cpp)

set(TESTS_SOURCES
    ${GLOB_SOURCES}
    PARENT_SCOPE)
