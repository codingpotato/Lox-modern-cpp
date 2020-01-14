set(SOURCES scanner.cpp to_string.cpp)

add_library(lox-interpreter ${SOURCES})
target_include_directories(lox-interpreter PUBLIC ../include)
target_compile_options(lox-interpreter
                       PUBLIC -Wall
                              -Werror
                              -Wextra
                              -Wpedantic
                              -pedantic-errors)
