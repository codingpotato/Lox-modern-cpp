mkdir -p build_benchmark && cd build_benchmark
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . -- -j8
./benchmark/lox_benchmark
