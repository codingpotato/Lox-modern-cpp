mkdir -p build && cd build
cmake -DCODE_COVERAGE=ON -DCMAKE_BUILD_TYPE=Debug ..
cmake --build . -- -j8
ctest -j8

lcov --directory . --capture --output-file coverage.info
lcov --remove coverage.info -o coverage.info    \
    '*/tools/*'                                 \
    '*/tests/*'                                 \
    '*/usr/include/*'
genhtml -o coverage coverage.info
