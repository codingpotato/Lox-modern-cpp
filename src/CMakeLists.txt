set(SOURCES main.cpp)

set(TESTS_DIR ../tests)
set(TESTS_SOURCES
    ${TESTS_DIR}/chunk_tests.cpp ${TESTS_DIR}/compiler_tests.cpp
    ${TESTS_DIR}/scanner_tests.cpp ${TESTS_DIR}/type_list_tests.cpp
    ${TESTS_DIR}/value_tests.cpp ${TESTS_DIR}/virtual_machine_tests.cpp)

add_executable(lox ${SOURCES} ${TESTS_SOURCES})
add_dependencies(lox doctest)
target_include_directories(lox PUBLIC ${PROJECT_SOURCE_DIR}/include
                                      ${DOCTEST_INCLUDE_DIR})
target_compile_options(lox PUBLIC -Wall -Werror -Wextra -Wpedantic
                                  -pedantic-errors)
doctest_discover_tests(lox)
