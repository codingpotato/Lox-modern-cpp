set(SOURCES main.cpp scanner.cpp to_string.cpp)

set(TESTS_DIR ../tests)
set(TESTS_SOURCES ${TESTS_DIR}/expression_tests.cpp
                  ${TESTS_DIR}/variant_tests.cpp)

add_executable(lox ${SOURCES} ${TESTS_SOURCES})
add_dependencies(lox doctest)
target_include_directories(lox PUBLIC ../include ${DOCTEST_INCLUDE_DIR})
target_compile_options(lox PUBLIC -Wall -Werror -Wextra -Wpedantic
                                  -pedantic-errors)
