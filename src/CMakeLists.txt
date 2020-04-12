set(SOURCES chunk.cpp value.cpp)

add_library(lox-core ${SOURCES})
target_include_directories(lox-core PUBLIC ${DOCTEST_DIR}
                                           ${PROJECT_SOURCE_DIR}/include)
target_compile_options(lox-core PUBLIC -Wall -Werror -Wextra -Wpedantic
                                       -pedantic-errors)
