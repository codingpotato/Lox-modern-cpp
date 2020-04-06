set(SOURCE
    equality.cpp
    fibonacci.cpp
    main.cpp
    hash_table_benchmark.cpp
    program_benchmark.cpp
    string_equality.cpp
    value_benchmark.cpp)

add_executable(lox-benchmark ${SOURCE})
target_include_directories(lox-benchmark PRIVATE ${BENCHMARK_DIR}/include
                                                 ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(lox-benchmark PRIVATE lox-core)
target_link_libraries(lox-benchmark PRIVATE benchmark)
target_link_libraries(lox-benchmark INTERFACE doctest)
