set(SOURCES main.cpp scanner.cpp to_string.cpp)

add_executable(lox ${SOURCES})
target_include_directories(lox PUBLIC ../include)
target_compile_options(lox PUBLIC -Wall -Werror -Wextra -Wpedantic
                                  -pedantic-errors)
