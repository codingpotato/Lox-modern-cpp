set(SOURCES chunk.cpp compiler.cpp scanner.cpp value.cpp vm.cpp)

add_library(lox_core ${SOURCES})
target_include_directories(lox_core PUBLIC ${DOCTEST_DIR} ${CMAKE_BINARY_DIR}
                                           ${PROJECT_SOURCE_DIR}/include)
target_compile_options(lox_core PUBLIC -Wall -Werror -Wextra -Wpedantic
                                       -pedantic-errors)
target_link_libraries(lox_core PUBLIC coverage_config)
