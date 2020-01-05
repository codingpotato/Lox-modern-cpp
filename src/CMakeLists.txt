add_executable(lox main.cpp)
target_compile_features(lox PRIVATE cxx_std_17)
target_compile_options(lox
                       PRIVATE -Wall
                               -Werror
                               -Wextra
                               -Wpedantic
                               -pedantic-errors)
